//
//  ngx_http_trackid.c
//  nginx-modules
//
//  Created by Flonly on 5/4/15.
//  Copyright (c) 2015 Flonly. All rights reserved.
//

#include "ngx_config.h"
#include "ngx_core.h"
#include <ngx_http_config.h>



#define NGX_HTTP_TRACKID_LEN  (32+1)

/* 31 Dec 2055 23:55:55 GMT+8 */
#define NGX_HTTP_TRACKID_MAX_EXPIRES  2713881355

static u_char expires[] = "; expires=Thu, 31-Dec-55 23:55:55 GMT";

typedef struct {
    ngx_str_t   cookie;
    ngx_uint_t  reset;
    
} ngx_http_trackid_ctx_t;

typedef struct  {
    ngx_str_t   trackid;
    ngx_str_t   domain;
    ngx_str_t   path;
    time_t      expires;
    
} ngx_http_trackid_conf_t;

extern ngx_http_output_header_filter_pt  ngx_http_top_header_filter;
static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;


/* 局部(本文件）函数生命 */
static ngx_int_t
ngx_http_trackid_init(ngx_conf_t *cf);
static ngx_http_trackid_ctx_t *
ngx_http_trackid_get(ngx_http_request_t *r,ngx_http_trackid_conf_t * conf);
static ngx_int_t
ngx_http_trackid_set(ngx_http_request_t *r,ngx_http_trackid_ctx_t * ctx,
                     ngx_http_trackid_conf_t * conf);
static ngx_int_t
ngx_http_trackid_create(ngx_http_request_t *r, ngx_http_trackid_ctx_t *ctx,
                        ngx_http_trackid_conf_t *conf);
static char *
ngx_http_trackid_expires(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void *
ngx_http_trackid_create_svr_conf(ngx_conf_t *cf);
static char *
ngx_http_trackid_merge_svr_conf(ngx_conf_t *cf, void *parent, void *child);



/* SHOULD static */
/* TODO  :  
            1.Http conf merge.
            2.domain
            3.expires
            4.type(algorithm)
            5.cookie path
*/
static ngx_command_t ngx_http_trackid_commands[] = {
    { ngx_string("trackid"),
      NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_trackid_conf_t,trackid),
      NULL                                 /* 设置参数是，需要的额外信息*/
    },
    { ngx_string("trackid_domain"),
        NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,
        NGX_HTTP_SRV_CONF_OFFSET,
        offsetof(ngx_http_trackid_conf_t,trackid),
        NULL                                 /* 设置参数是，需要的额外信息*/
    },
    { ngx_string("trackid_path"),
        NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,
        NGX_HTTP_SRV_CONF_OFFSET,
        offsetof(ngx_http_trackid_conf_t,trackid),
        NULL                                 /* 设置参数是，需要的额外信息*/
    },
    { ngx_string("trackid_expires"),
        NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
        ngx_http_trackid_expires,
        NGX_HTTP_SRV_CONF_OFFSET,
        0,                                   /* 自定义解析函数无需offset */
        NULL                                 /* 设置参数是，需要的额外信息*/
    },

    ngx_null_command    /*数组的结束符*/
};

static ngx_int_t
ngx_http_trackid_init(ngx_conf_t *cf);
/* 这里 SHOULD 用 static, 标明这是一个本地（文件范围）变量*/
static ngx_http_module_t ngx_http_trackid_module_ctx ={
     NULL,                              //    ngx_int_t   (*preconfiguration)(ngx_conf_t *cf);
     ngx_http_trackid_init,             //    ngx_int_t   (*postconfiguration)(ngx_conf_t *cf);
    
     NULL,                              //    void       *(*create_main_conf)(ngx_conf_t *cf);
     NULL,                              //    char       *(*init_main_conf)(ngx_conf_t *cf, void *conf);

     ngx_http_trackid_create_svr_conf,  //    void       *(*create_srv_conf)(ngx_conf_t *cf);
     ngx_http_trackid_merge_svr_conf,   //    char       *(*merge_srv_conf)(ngx_conf_t *cf, void *prev, void *conf);
    
     NULL,                              //    void       *(*create_loc_conf)(ngx_conf_t *cf);
     NULL                               //    char       *(*merge_loc_conf)(ngx_conf_t *cf, void *prev, void *conf);
};


//struct ngx_module_s {
//    ngx_uint_t            ctx_index；
//    /*分类的模块计数器
//     nginx模块可以分为四种：core、event、http和mail
//     每个模块都会各自计数，ctx_index就是每个模块在其所属类组的计数*/
//    
//    ngx_uint_t            index;
//    /*一个模块计数器，按照每个模块在ngx_modules[]数组中的声明顺序，从0开始依次给每个模块赋值*/
//    
//    ngx_uint_t            spare0;
//    ngx_uint_t            spare1;
//    ngx_uint_t            spare2;
//    ngx_uint_t            spare3;
//    
//    ngx_uint_t            version;      //nginx模块版本
//    
//    void                 *ctx;
//    /*模块的上下文，不同种类的模块有不同的上下文，因此实现了四种结构体*/
//    
//    ngx_command_t        *commands;
//    /*命令定义地址
//     模块的指令集
//     每一个指令在源码中对应着一个ngx_command_t结构变量*/
//    
//    ngx_uint_t            type;         //模块类型，用于区分core event http和mail
//    
//    ngx_int_t           (*init_master)(ngx_log_t *log);         //初始化master时执行
//    
//    ngx_int_t           (*init_module)(ngx_cycle_t *cycle);     //初始化module时执行
//    
//    ngx_int_t           (*init_process)(ngx_cycle_t *cycle);    //初始化process时执行
//    ngx_int_t           (*init_thread)(ngx_cycle_t *cycle);     //初始化thread时执行
//    void                (*exit_thread)(ngx_cycle_t *cycle);     //退出thread时执行
//    void                (*exit_process)(ngx_cycle_t *cycle);    //退出process时执行
//    
//    void                (*exit_master)(ngx_cycle_t *cycle);     //退出master时执行
//    
//    //以下功能不明
//    uintptr_t             spare_hook0;
//    uintptr_t             spare_hook1;
//    uintptr_t             spare_hook2;
//    uintptr_t             spare_hook3;
//    uintptr_t             spare_hook4;
//    uintptr_t             spare_hook5;
//    uintptr_t             spare_hook6;
//    uintptr_t             spare_hook7;
//};
//
//typedef struct ngx_module_s      ngx_module_t;
/* 这里需要定义一个全局变量 ngx_http_trackid 模块，在nginx的autoconf 里面要用到*/
ngx_module_t  ngx_http_trackid_filter_module = {
    NGX_MODULE_V1,
    &ngx_http_trackid_module_ctx,          /* module context */
    ngx_http_trackid_commands,             /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_int_t
ngx_http_trackid_filter(ngx_http_request_t *r)
{
    //1.check 是否已经有了cookie
    //2.设置新cookie
    
    ngx_http_trackid_ctx_t   *ctx;
    ngx_http_trackid_conf_t  *conf;
    
    /* 只处理主请求 */
    if (r != r->main)     {
        return ngx_http_next_header_filter(r);
    }
    
    conf = ngx_http_get_module_loc_conf(r, ngx_http_trackid_filter_module);
    //读取当前值
    ctx = ngx_http_trackid_get(r, conf);
    if (ctx == NULL) {
        return NGX_ERROR;
    }
    
    //设置新值
    if (ngx_http_trackid_set(r, ctx, conf) == NGX_OK) {
        return ngx_http_next_header_filter(r);
    }
    
    return NGX_ERROR;
}


static ngx_http_trackid_ctx_t *
ngx_http_trackid_get(ngx_http_request_t *r,ngx_http_trackid_conf_t * conf)
{
    ngx_int_t                   n;
    ngx_table_elt_t             **cookies;
    ngx_http_trackid_ctx_t      *ctx;
    
    ctx = ngx_http_get_module_ctx(r, ngx_http_trackid_filter_module);
    
    if (ctx) {
        return ctx;
    }
    
    if (ctx == NULL) {
        ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_trackid_conf_t));
        if (ctx == NULL) {
            return NULL;
        }
        
        ngx_http_set_ctx(r, ctx, ngx_http_trackid_filter_module);
    }
    
    n = ngx_http_parse_multi_header_lines(&r->headers_in.cookies, &conf->trackid,
                                          &ctx->cookie);
    if (n == NGX_DECLINED) {
        return ctx;
    }
    
    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "uid cookie: \"%V\"", &ctx->cookie);
    
    if (ctx->cookie.len != NGX_HTTP_TRACKID_LEN) {
        cookies = r->headers_in.cookies.elts;
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                      "client sent cookie with error length \"%V\"",
                      &cookies[n]->value);
    }
    
    return ctx;
}



static ngx_int_t
ngx_http_trackid_set(ngx_http_request_t *r,ngx_http_trackid_ctx_t * ctx,
                     ngx_http_trackid_conf_t * conf)
{
    u_char           *cookie, *p;
    size_t            len;
    ngx_table_elt_t  *set_cookie;
    
    if (ngx_http_trackid_create(r, ctx, conf) != NGX_OK) {
        return NGX_ERROR;
    }
    
    len = conf->trackid.len + 1 + NGX_HTTP_TRACKID_LEN + conf->path.len;
    
    if (conf->expires) {
        len += sizeof(expires) - 1 + 2;
    }
    
    if (conf->domain.len) {
        len += conf->domain.len;
    }
    
    cookie = ngx_pnalloc(r->pool, len);
    if (cookie == NULL) {
        return NGX_ERROR;
    }
    
    p = ngx_copy(cookie, conf->trackid.data, conf->trackid.len);
    *p++ = '=';
    p = ngx_cpymem(p, ctx->cookie.data,NGX_HTTP_TRACKID_LEN);
    
    if (conf->expires == NGX_HTTP_TRACKID_MAX_EXPIRES) {
        p = ngx_cpymem(p, expires, sizeof(expires) - 1);
        
    } else if (conf->expires) {
        p = ngx_cpymem(p, expires, sizeof("; expires=") - 1);
        p = ngx_http_cookie_time(p, ngx_time() + conf->expires);
    }
    
    p = ngx_copy(p, conf->domain.data, conf->domain.len);
    
    p = ngx_copy(p, conf->path.data, conf->path.len);
    
    set_cookie = ngx_list_push(&r->headers_out.headers);
    if (set_cookie == NULL) {
        return NGX_ERROR;
    }
    
    set_cookie->hash = 1;
    ngx_str_set(&set_cookie->key, "Set-Cookie");
    set_cookie->value.len = p - cookie;
    set_cookie->value.data = cookie;
    
    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "uid cookie: \"%V\"", &set_cookie->value);
    
    return NGX_OK;

}


/* 自己添加在 http 的header 过滤链表里面  */
static ngx_int_t
ngx_http_trackid_init(ngx_conf_t *cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_trackid_filter;
    
    return NGX_OK;
}


static ngx_int_t
ngx_http_trackid_create(ngx_http_request_t *r, ngx_http_trackid_ctx_t *ctx,
                        ngx_http_trackid_conf_t *conf)
{
    ctx->cookie.len = NGX_HTTP_TRACKID_LEN;
    ctx->cookie.data= "12345678abcdefgh12345678abcdefgh";
    
    return NGX_OK;
}


static char *
ngx_http_trackid_expires(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_trackid_conf_t *tcf = conf;
    
    ngx_str_t  *value;
    
    if (tcf->expires != NGX_CONF_UNSET) {
        return "is duplicate";
    }
    
    value = cf->args->elts;
    
    if (ngx_strcmp(value[1].data, "max") == 0) {
        tcf->expires = NGX_HTTP_TRACKID_MAX_EXPIRES;
        return NGX_CONF_OK;
    }
    
    if (ngx_strcmp(value[1].data, "off") == 0) {
        tcf->expires = 0;
        return NGX_CONF_OK;
    }
    
    tcf->expires = ngx_parse_time(&value[1], 1);
    if (tcf->expires == (time_t) NGX_ERROR) {
        return "invalid value";
    }
    
    return NGX_CONF_OK;
}



static void *
ngx_http_trackid_create_svr_conf(ngx_conf_t *cf)
{
    ngx_http_trackid_conf_t  *conf;
    
    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_trackid_conf_t));
    if (conf == NULL) {
        return NULL;
    }
    
    /*
     * set by ngx_pcalloc():
     *
     *     conf->name = { 0, NULL };
     *     conf->domain = { 0, NULL };
     *     conf->path = { 0, NULL };
     */
    
    conf->expires = NGX_CONF_UNSET;
    
    return conf;
}

static char *
ngx_http_trackid_merge_svr_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_trackid_conf_t *prev = parent;
    ngx_http_trackid_conf_t *conf = child;
    
    ngx_conf_merge_str_value(conf->trackid, prev->trackid, "BAIDUID");
    ngx_conf_merge_str_value(conf->domain, prev->domain, "");
    ngx_conf_merge_str_value(conf->path, prev->path, "; path=/");
    ngx_conf_merge_sec_value(conf->expires, prev->expires, 0);
    
    return NGX_CONF_OK;
}










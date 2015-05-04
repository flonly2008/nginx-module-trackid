//
//  ngx_http_trackid.c
//  nginx-modules
//
//  Created by Flonly on 5/4/15.
//  Copyright (c) 2015 Flonly. All rights reserved.
//

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http_config.h>

typedef struct  {
    ngx_str_t trackid;
    
} ngx_http_trackid_conf_t;


/* SHOULD static */
static ngx_command_t ngx_http_trackid_commands[] = {
    { ngx_string("trackid"),
      NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_trackid_conf_t,trackid),
      NULL                                 /* 设置参数是，需要的额外信息*/
    },
    ngx_null_command    /*数组的结束符*/
};

/* 这里 SHOULD 用 static, 标明这是一个本地（文件范围）变量*/
static ngx_http_module_t ngx_http_trackid_module_ctx ={
     NULL,                          //    ngx_int_t   (*preconfiguration)(ngx_conf_t *cf);
     ngx_http_trackid_init,         //    ngx_int_t   (*postconfiguration)(ngx_conf_t *cf);
            //
            //    void       *(*create_main_conf)(ngx_conf_t *cf);
            //    char       *(*init_main_conf)(ngx_conf_t *cf, void *conf);
            //
            //    void       *(*create_srv_conf)(ngx_conf_t *cf);
            //    char       *(*merge_srv_conf)(ngx_conf_t *cf, void *prev, void *conf);
            //
            //    void       *(*create_loc_conf)(ngx_conf_t *cf);
            //    char       *(*merge_loc_conf)(ngx_conf_t *cf, void *prev, void *conf);
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
}


/* 自己添加在 http 的header 过滤链表里面  */
extern ngx_http_output_header_filter_pt  ngx_http_top_header_filter;
static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;
static ngx_uint_t
ngx_http_trackid_init(ngx_conf_t *cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_trackid_filter;
    
    return NGX_OK;
}
















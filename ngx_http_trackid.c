//
//  ngx_http_trackid.c
//  nginx-modules
//
//  Created by Flonly on 5/4/15.
//  Copyright (c) 2015 Flonly. All rights reserved.
//

#include <ngx_config.h>
#include <ngx_core.h>






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

ngx_module_t ngx_http_trackid = {
    NGX_MODULE_V1,
    
    
}
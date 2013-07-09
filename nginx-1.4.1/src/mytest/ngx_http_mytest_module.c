#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <ngx_list.h>
#include <ngx_http_request.h>

#include "fileOperating.h"
/*extern void createFile(char * fileName);*/
/*

HTTP框架在NGX_HTTP_CONTENT_PHASE阶段就会调用到我们实现的ngx_http_mytest_handler方法来处理这个用户请求。事实上，HTTP框架共定义了11个阶段（第三方HTTP模块只能介入其中的7个阶段处理请求，详见10.6节），本章只关注NGX_HTTP_CONTENT_PHASE处理阶段，多数HTTP模块都在此阶段实现相关功能。下面简单说明一下这11个阶段。

*/
static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r) ;
static char *  
ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)  
{  
    ngx_http_core_loc_conf_t  *clcf;  
 
    /*首先找到mytest配置项所属的配置块，clcf看上去像是location块内的数据结构，其实不然，它可以是main、srv或者loc级别配置项，也就是说，在每个http{}和server{}内也都有一个ngx_http_core_loc_conf_t结构体*/  
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);  
 
    /*HTTP框架在处理用户请求进行到NGX_HTTP_CONTENT_PHASE阶段时，如果请求的主机域名、URI与mytest配置项所在的配置块相匹配，就将调用我们实现的ngx_http_mytest_handler方法处理这个请求*/  
/*这个返回值可以是HTTP中响应包的返回码，其中包括了HTTP框架已经在/src/http/ngx_http_request.h文件中定义好的宏，如下所示。*/
    clcf->handler = ngx_http_mytest_handler;  
 
    return NGX_CONF_OK;  
} 





/*commands数组用于定义模块的配置文件参数，每一个数组元素都是ngx_command_t类型，数组的结尾用ngx_null_command表示。Nginx在解析配置文件中的一个配置项时首先会遍历所有的模块，对于每一个模块而言，即通过遍历commands数组进行，另外，在数组中检查到ngx_null_command时，会停止使用当前模块解析该配置项，每一个ngx_command_t结构体定义了自己感兴趣的一个配置项：*/
static ngx_command_t  ngx_http_mytest_commands[] = {  
 
    { ngx_string("mytest"),  
NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_NOARGS,  /*配置项类型，type将指定配置项可以出现的位置*/
      ngx_http_mytest,  /*    //出现了name中指定的配置项后，将会调用set方法处理配置项的参数  ，就是配置文件的某个项目的值
*/
      NGX_HTTP_LOC_CONF_OFFSET,  /*通常用于使用预设的解析方法解析配置项，这是配置模块的一个优秀设计。它需要与conf配合使用，在第4章中详细介绍*/  
      0,  /*//在配置文件中的偏移量  */
      NULL /*//配置项读取后的处理方法，必须是ngx_conf_post_t结构的指针 */
     },  
 
      ngx_null_command  
}; 

/*HTTP框架在读取、重载配置文件时定义了由ngx_http_module_t接口描述的8个阶段，HTTP框架在启动过程中会在每个阶段中调用ngx_http_module_t中相应的方法。当然，如果ngx_http_module_t中的某个回调方法设为NULL空指针，那么HTTP框架是不会调用它的。*/
static ngx_http_module_t  ngx_http_mytest_module_ctx = {  
    NULL,                       /* preconfiguration */  
    NULL,                   /* postconfiguration */  
 
    NULL,                       /* create main configuration */  
    NULL,                       /* init main configuration */  
 
    NULL,                       /* create server configuration */  
    NULL,                       /* merge server configuration */  
 
    NULL,                   /* create location configuration */  
    NULL                    /* merge location configuration */  
}; 

ngx_module_t  ngx_http_mytest_module = {  
    NGX_MODULE_V1,  
    &ngx_http_mytest_module_ctx,           /* module context */  
    ngx_http_mytest_commands,              /* module directives */  /* ctx用于指向一类模块的上下文结构体，为什么需要ctx呢？因为前面说过，Nginx模块有许多种类，不同类模块之间的功能差别很大。例如，事件类型的模块主要处理I/O事件相关的功能，HTTP类型的模块主要处理HTTP应用层的功能。这样，每个模块都有了自己的特性，而ctx将会指向特定类型模块的公共接口。例如，在HTTP模块中，ctx需要指向ngx_http_module_t结构体*/  
    NGX_HTTP_MODULE,                       /* module type */  /*/*type表示该模块的类型，它与ctx指针是紧密相关的。在官方Nginx中，它的取值范围是以下5种：NGX_HTTP_MODULE、NGX_CORE_MODULE、NGX_CONF_MODULE、NGX_EVENT_MODULE、NGX_MAIL_MODULE。这5种模块间的关系参考图8-2。实际上，还可以自定义新的模块类型*/  
    NULL,                                  /* init master */  /*下面是在Nginx的启动、停止过程中，以下7个函数指针表示有7个执行点会分别调用这7种方法*/
    NULL,                                  /* init module */  
    NULL,                                  /* init process */  
    NULL,                                  /* init thread */  
    NULL,                                  /* exit thread */  
    NULL,                                  /* exit process */  
    NULL,                                  /* exit master */  
    NGX_MODULE_V1_PADDING  		  /*保留字段*/
}; 
static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r)  
{  

    createFile("/tmp/nginxtmp");
/*    createFile("/tmp/nginxtmp2");*/
/*	FILE * myfile=fopen("/tmp/nginxtmp", "w+");*/
/*	fclose(myfile);*/

    //必须是GET或者HEAD方法，否则返回405 Not Allowed  
    if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD))) {  
        return NGX_HTTP_NOT_ALLOWED;  
    }  
 /*何必还要调用ngx_http_discard_request_body方法呢？其实这一步非常有意义，因为有些客户端可能会一直试图发送包体，而如果HTTP模块不接收发来的TCP流，有可能造成客户端发送超时。*/
    //丢弃请求中的包体  
    ngx_int_t rc = ngx_http_discard_request_body(r);  
    if (rc != NGX_OK) {  
        return rc;  
    }  
 
    /*设置返回的Content-Type。注意，ngx_str_t有一个很方便的初始化宏ngx_string，它可以把ngx_str_t的data和len成员都设置好*/  
    ngx_str_t type = ngx_string("text/plain");  
    //返回的包体内容  
    ngx_str_t response = ngx_string("Hello World!");  
    //设置返回状态码  
    r->headers_out.status = NGX_HTTP_OK;  
    //响应包是有包体内容的，需要设置Content-Length长度  
    r->headers_out.content_length_n = response.len;  
    //设置Content-Type  
    r->headers_out.content_type = type;  
 
    //发送HTTP头部  
    rc = ngx_http_send_header(r);  
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {  
        return rc;  
    }  
 
    //构造ngx_buf_t结构体准备发送包体  
    ngx_buf_t *b;  
    b = ngx_create_temp_buf(r->pool, response.len);  
    if (b == NULL) {  
        return NGX_HTTP_INTERNAL_SERVER_ERROR;  
    }  
    //将Hello World复制到ngx_buf_t指向的内存中  
    ngx_memcpy(b->pos, response.data, response.len);  
    //注意，一定要设置好last指针  
    b->last = b->pos + response.len;  
    //声明这是最后一块缓冲区  
    b->last_buf = 1;  
 
    //构造发送时的ngx_chain_t结构体  
    ngx_chain_t out;  
    //赋值ngx_buf_t  
    out.buf = b;  
    //设置next为NULL  
    out.next = NULL;  
 
    /*最后一步为发送包体，发送结束后HTTP框架会调用ngx_http_finalize_request方法结束请求*/  
    return ngx_http_output_filter(r, &out);  
} 








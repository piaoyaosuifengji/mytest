#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <ngx_list.h>
#include <ngx_http_request.h>

static ngx_int_t ngx_http_filetransfer_handler2(ngx_http_request_t *r) ;
/*void ngx_pool_cleanup_file(void *data)  */
/*{  */
/*    ngx_pool_cleanup_file_t  *c = data;  */
/* */
/*    ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, c->log, 0, "file cleanup: fd:%d",c->fd);  */
/* */
/*    if (ngx_close_file(c->fd) == NGX_FILE_ERROR) */
/*    {  */
/*        ngx_log_error(NGX_LOG_ALERT, c->log, ngx_errno,  */
/*                      ngx_close_file_n " \"%s\" failed", c->name);  */
/*    }  */
/*} */


static char *  
ngx_http_filetransfer(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)  
{  
    ngx_http_core_loc_conf_t  *clcf;  
 
    /*首先找到filetransfer配置项所属的配置块，clcf看上去像是location块内的数据结构，其实不然，它可以是main、srv或者loc级别配置项，也就是说，在每个http{}和server{}内也都有一个ngx_http_core_loc_conf_t结构体*/  
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);  
 
    /*HTTP框架在处理用户请求进行到NGX_HTTP_CONTENT_PHASE阶段时，如果请求的主机域名、URI与filetransfer配置项所在的配置块相匹配，就将调用我们实现的ngx_http_filetransfer_handler方法处理这个请求*/  
/*这个返回值可以是HTTP中响应包的返回码，其中包括了HTTP框架已经在/src/http/ngx_http_request.h文件中定义好的宏，如下所示。*/
    clcf->handler = ngx_http_filetransfer_handler2;  
 
    return NGX_CONF_OK;  
} 





/*commands数组用于定义模块的配置文件参数，每一个数组元素都是ngx_command_t类型，数组的结尾用ngx_null_command表示。Nginx在解析配置文件中的一个配置项时首先会遍历所有的模块，对于每一个模块而言，即通过遍历commands数组进行，另外，在数组中检查到ngx_null_command时，会停止使用当前模块解析该配置项，每一个ngx_command_t结构体定义了自己感兴趣的一个配置项：*/
static ngx_command_t  ngx_http_filetransfer_commands[] = {  
 
    { ngx_string("filetransfer"),  
NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_NOARGS,  /*配置项类型，type将指定配置项可以出现的位置*/
      ngx_http_filetransfer,  /*    //出现了name中指定的配置项后，将会调用set方法处理配置项的参数  ，就是配置文件的某个项目的值
*/
      NGX_HTTP_LOC_CONF_OFFSET,  /*通常用于使用预设的解析方法解析配置项，这是配置模块的一个优秀设计。它需要与conf配合使用，在第4章中详细介绍*/  
      0,  /*//在配置文件中的偏移量  */
      NULL /*//配置项读取后的处理方法，必须是ngx_conf_post_t结构的指针 */
     },  
 
      ngx_null_command  
}; 

/*HTTP框架在读取、重载配置文件时定义了由ngx_http_module_t接口描述的8个阶段，HTTP框架在启动过程中会在每个阶段中调用ngx_http_module_t中相应的方法。当然，如果ngx_http_module_t中的某个回调方法设为NULL空指针，那么HTTP框架是不会调用它的。*/
static ngx_http_module_t  ngx_http_filetransfer_module_ctx = {  
    NULL,                       /* preconfiguration */  
    NULL,                   /* postconfiguration */  
 
    NULL,                       /* create main configuration */  
    NULL,                       /* init main configuration */  
 
    NULL,                       /* create server configuration */  
    NULL,                       /* merge server configuration */  
 
    NULL,                   /* create location configuration */  
    NULL                    /* merge location configuration */  
}; 

ngx_module_t  ngx_http_filetransfer_module = {  
    NGX_MODULE_V1,  
    &ngx_http_filetransfer_module_ctx,           /* module context */  
    ngx_http_filetransfer_commands,              /* module directives */  /* ctx用于指向一类模块的上下文结构体，为什么需要ctx呢？因为前面说过，Nginx模块有许多种类，不同类模块之间的功能差别很大。例如，事件类型的模块主要处理I/O事件相关的功能，HTTP类型的模块主要处理HTTP应用层的功能。这样，每个模块都有了自己的特性，而ctx将会指向特定类型模块的公共接口。例如，在HTTP模块中，ctx需要指向ngx_http_module_t结构体*/  
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
static ngx_int_t ngx_http_filetransfer_handler2(ngx_http_request_t *r)  
{  
    //必须是GET或者HEAD方法，否则返回405 Not Allowed  
    if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD))) {  
        return NGX_HTTP_NOT_ALLOWED;  
    }  

    //丢弃请求中的包体  
    ngx_int_t rc = ngx_http_discard_request_body(r);  
    if (rc != NGX_OK) {  
        return rc;  
    }  
 

    ngx_str_t type = ngx_string("text/html");  
    //返回的包体内容  

    //设置返回状态码  
    r->headers_out.status = NGX_HTTP_OK;  
    //响应包是有包体内容的，需要设置Content-Length长度  

    //设置Content-Type  
    r->headers_out.content_type = type;  
 
    //发送HTTP头部  
    rc = ngx_http_send_header(r);  
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {  
        return rc;  
    }  
 



    ngx_buf_t *b; 
    b = ngx_palloc(r->pool, sizeof(ngx_buf_t)); 
    b->in_file=1;
    u_char* filename = (u_char*)"/usr/local/nginx/html/Google.html";  
    b->file = ngx_pcalloc(r->pool, sizeof(ngx_file_t));  
    b->file->fd = ngx_open_file(filename, NGX_FILE_RDONLY|NGX_FILE_NONBLOCK, NGX_FILE_OPEN, 0);  
    b->file->log = r->connection->log;  
    b->file->name.data = filename;  
    b->file->name.len = sizeof(filename)-1;  
	if (b->file->fd <= 0)  
	{  
	    return NGX_HTTP_NOT_FOUND;  
	} 
    
    if (ngx_file_info(filename, &b->file->info) == NGX_FILE_ERROR) 
    {  
        return NGX_HTTP_INTERNAL_SERVER_ERROR;  
    } 

    r->headers_out.content_length_n = b->file->info.st_size; 
    b->file_pos = 0;  
    b->file_last = b->file->info.st_size; 
   
    ngx_pool_cleanup_t* cln = ngx_pool_cleanup_add(r->pool, sizeof(ngx_pool_cleanup_file_t));  
    if (cln == NULL) {  
    return NGX_ERROR;  
    }  
     
    cln->handler = ngx_pool_cleanup_file;  
    ngx_pool_cleanup_file_t  *clnclnf = cln->data;  
 
    clnclnf->fd = b->file->fd;  
    clnclnf->name = b->file->name.data;  
    clnclnf->log = r->pool->log; 


    //构造发送时的ngx_chain_t结构体  
    ngx_chain_t out;  
    //赋值ngx_buf_t  
    out.buf = b;  
    //设置next为NULL  
    out.next = NULL;  
 
     
    return ngx_http_output_filter(r, &out);  
} 







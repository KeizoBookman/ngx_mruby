/*
// ngx_http_mruby_http2.c - ngx_mruby mruby module
//
// See Copyright Notice in ngx_http_mruby_module.c
*/

#include "ngx_http_mruby_module.h"
#include "ngx_http_mruby_request.h"
#include "ngx_http_v2_module.h"
#include "ngx_http_mruby_http2.h"

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>

typedef struct {
    ngx_http_v2_loc_conf_t *lcf;
}ngx_mrb_http_v2_data;

static void ngx_mrb_http_v2_free(mrb_state *mrb, void *p)
{
    ngx_mrb_http_v2_data *d = p;
    d->lcf =NULL;
    mrb_free(mrb,d);

}

static const struct mrb_data_type ngx_mrb_http_v2_data_type = {
    "ngx_mruby_http_v2_data", ngx_mrb_http_v2_free,
};

static mrb_value ngx_mrb_http_v2_init(mrb_state *mrb, mrb_value self)
{
    ngx_http_request_t *r;
    ngx_http_v2_loc_conf_t *v2lcf;
    ngx_mrb_http_v2_data *data = DATA_PTR(self);
    if(!data)
    {
        ngx_mrb_http_v2_free(mrb, data);
    }
    data = (ngx_mrb_http_v2_data *)mrb_malloc(mrb, sizeof(ngx_mrb_http_v2_data));

    // TODO: maybe it is incorrect access
    r = ngx_mrb_get_request();
    if(!r)
    {
        mrb_raise(mrb, E_RUNTIME_ERROR, "ngx_http_request is NULL");
    }
    v2lcf = ngx_http_get_module_loc_conf(r, ngx_http_v2_module);
    if(!v2lcf)
    {
        mrb_raise(mrb, E_RUNTIME_ERROR, "ngx_http_v2_module is not found");
    }

    data->lcf = v2lcf;
    mrb_data_init(self, data, &ngx_mrb_http_v2_data_type);
    return self;
}

static mrb_value ngx_mrb_http_v2_enable_server_push_preload(mrb_state *mrb, mrb_value self)
{
    ngx_http_v2_loc_conf_t *v2lcf;
    ngx_mrb_http_v2_data *data;
    data =DATA_PTR(self);
    v2lcf = data->lcf;
    if(!v2lcf)
    {
        mrb_raise(mrb, E_RUNTIME_ERROR, "ngx_http_v2_module location config is NULL");
    }
    v2lcf->push_preload = 1;
    return self;
}

static mrb_value ngx_mrb_http_v2_disable_server_push_preload(mrb_state *mrb, mrb_value self)
{
    ngx_http_v2_loc_conf_t *v2lcf;
    ngx_mrb_http_v2_data *data;
    data =DATA_PTR(self);
    v2lcf = data->lcf;
    if(!v2lcf)
    {
        mrb_raise(mrb, E_RUNTIME_ERROR, "ngx_http_v2_module location config is NULL");
    }
    v2lcf->push_preload = 0;
    return self;
}

static mrb_value ngx_mrb_http_v2_get_server_push_preload(mrb_state *mrb, mrb_value self)
{
    ngx_http_v2_loc_conf_t *v2lcf;
    ngx_mrb_http_v2_data *data;
    data =DATA_PTR(self);
    v2lcf = data->lcf;
    if(!v2lcf)
    {
        mrb_raise(mrb, E_RUNTIME_ERROR, "ngx_http_v2_module location config is NULL");
    }
    return v2lcf->push_preload ? mrb_true_value() : mrb_false_value();
}

void ngx_mrb_http_v2_class_init(mrb_state *mrb, struct RClass *calss)
{
    struct RClass *class_http2;

    class_http2 = mrb_define_class_under(mrb, calss, "HTTP2", mrb->object_class);
    mrb_define_method(mrb, class_http2, "initialize", ngx_mrb_http_v2_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, class_http2, "enable_server_push_preload", ngx_mrb_http_v2_enable_server_push_preload, MRB_ARGS_NONE());
    mrb_define_method(mrb, class_http2, "disable_server_push_preload", ngx_mrb_http_v2_disable_server_push_preload, MRB_ARGS_NONE());
    mrb_define_method(mrb, class_http2, "server_push_preload", ngx_mrb_http_v2_get_server_push_preload, MRB_ARGS_NONE());
}

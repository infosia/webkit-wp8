/*
 *  This file is part of the WebKit open source project.
 *  This file has been generated by generate-bindings.pl. DO NOT MODIFY!
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "WebKitDOMTestNondeterministic.h"

#include "CSSImportRule.h"
#include "DOMObjectCache.h"
#include "Document.h"
#include "ExceptionCode.h"
#include "JSMainThreadExecState.h"
#include "WebKitDOMPrivate.h"
#include "WebKitDOMTestNondeterministicPrivate.h"
#include "gobject/ConvertToUTF8String.h"
#include <wtf/GetPtr.h>
#include <wtf/RefPtr.h>

#define WEBKIT_DOM_TEST_NONDETERMINISTIC_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, WEBKIT_TYPE_DOM_TEST_NONDETERMINISTIC, WebKitDOMTestNondeterministicPrivate)

typedef struct _WebKitDOMTestNondeterministicPrivate {
    RefPtr<WebCore::TestNondeterministic> coreObject;
} WebKitDOMTestNondeterministicPrivate;

namespace WebKit {

WebKitDOMTestNondeterministic* kit(WebCore::TestNondeterministic* obj)
{
    if (!obj)
        return 0;

    if (gpointer ret = DOMObjectCache::get(obj))
        return WEBKIT_DOM_TEST_NONDETERMINISTIC(ret);

    return wrapTestNondeterministic(obj);
}

WebCore::TestNondeterministic* core(WebKitDOMTestNondeterministic* request)
{
    return request ? static_cast<WebCore::TestNondeterministic*>(WEBKIT_DOM_OBJECT(request)->coreObject) : 0;
}

WebKitDOMTestNondeterministic* wrapTestNondeterministic(WebCore::TestNondeterministic* coreObject)
{
    ASSERT(coreObject);
    return WEBKIT_DOM_TEST_NONDETERMINISTIC(g_object_new(WEBKIT_TYPE_DOM_TEST_NONDETERMINISTIC, "core-object", coreObject, NULL));
}

} // namespace WebKit

G_DEFINE_TYPE(WebKitDOMTestNondeterministic, webkit_dom_test_nondeterministic, WEBKIT_TYPE_DOM_OBJECT)

enum {
    PROP_0,
    PROP_NONDETERMINISTIC_READONLY_ATTR,
    PROP_NONDETERMINISTIC_WRITEABLE_ATTR,
    PROP_NONDETERMINISTIC_EXCEPTION_ATTR,
    PROP_NONDETERMINISTIC_GETTER_EXCEPTION_ATTR,
    PROP_NONDETERMINISTIC_SETTER_EXCEPTION_ATTR,
};

static void webkit_dom_test_nondeterministic_finalize(GObject* object)
{
    WebKitDOMTestNondeterministicPrivate* priv = WEBKIT_DOM_TEST_NONDETERMINISTIC_GET_PRIVATE(object);

    WebKit::DOMObjectCache::forget(priv->coreObject.get());

    priv->~WebKitDOMTestNondeterministicPrivate();
    G_OBJECT_CLASS(webkit_dom_test_nondeterministic_parent_class)->finalize(object);
}

static void webkit_dom_test_nondeterministic_set_property(GObject* object, guint propertyId, const GValue* value, GParamSpec* pspec)
{
    WebCore::JSMainThreadNullState state;
    WebKitDOMTestNondeterministic* self = WEBKIT_DOM_TEST_NONDETERMINISTIC(object);
    WebCore::TestNondeterministic* coreSelf = WebKit::core(self);

    switch (propertyId) {
    case PROP_NONDETERMINISTIC_WRITEABLE_ATTR: {
        coreSelf->setNondeterministicWriteableAttr(WTF::String::fromUTF8(g_value_get_string(value)));
        break;
    }
    case PROP_NONDETERMINISTIC_EXCEPTION_ATTR: {
        coreSelf->setNondeterministicExceptionAttr(WTF::String::fromUTF8(g_value_get_string(value)));
        break;
    }
    case PROP_NONDETERMINISTIC_GETTER_EXCEPTION_ATTR: {
        coreSelf->setNondeterministicGetterExceptionAttr(WTF::String::fromUTF8(g_value_get_string(value)));
        break;
    }
    case PROP_NONDETERMINISTIC_SETTER_EXCEPTION_ATTR: {
        WebCore::ExceptionCode ec = 0;
        coreSelf->setNondeterministicSetterExceptionAttr(WTF::String::fromUTF8(g_value_get_string(value)), ec);
        break;
    }
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propertyId, pspec);
        break;
    }
}

static void webkit_dom_test_nondeterministic_get_property(GObject* object, guint propertyId, GValue* value, GParamSpec* pspec)
{
    WebCore::JSMainThreadNullState state;
    WebKitDOMTestNondeterministic* self = WEBKIT_DOM_TEST_NONDETERMINISTIC(object);
    WebCore::TestNondeterministic* coreSelf = WebKit::core(self);

    switch (propertyId) {
    case PROP_NONDETERMINISTIC_READONLY_ATTR: {
        g_value_set_long(value, coreSelf->nondeterministicReadonlyAttr());
        break;
    }
    case PROP_NONDETERMINISTIC_WRITEABLE_ATTR: {
        g_value_take_string(value, convertToUTF8String(coreSelf->nondeterministicWriteableAttr()));
        break;
    }
    case PROP_NONDETERMINISTIC_EXCEPTION_ATTR: {
        g_value_take_string(value, convertToUTF8String(coreSelf->nondeterministicExceptionAttr()));
        break;
    }
    case PROP_NONDETERMINISTIC_GETTER_EXCEPTION_ATTR: {
        WebCore::ExceptionCode ec = 0;
        g_value_take_string(value, convertToUTF8String(coreSelf->nondeterministicGetterExceptionAttr(ec)));
        break;
    }
    case PROP_NONDETERMINISTIC_SETTER_EXCEPTION_ATTR: {
        g_value_take_string(value, convertToUTF8String(coreSelf->nondeterministicSetterExceptionAttr()));
        break;
    }
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propertyId, pspec);
        break;
    }
}

static GObject* webkit_dom_test_nondeterministic_constructor(GType type, guint constructPropertiesCount, GObjectConstructParam* constructProperties)
{
    GObject* object = G_OBJECT_CLASS(webkit_dom_test_nondeterministic_parent_class)->constructor(type, constructPropertiesCount, constructProperties);

    WebKitDOMTestNondeterministicPrivate* priv = WEBKIT_DOM_TEST_NONDETERMINISTIC_GET_PRIVATE(object);
    priv->coreObject = static_cast<WebCore::TestNondeterministic*>(WEBKIT_DOM_OBJECT(object)->coreObject);
    WebKit::DOMObjectCache::put(priv->coreObject.get(), object);

    return object;
}

static void webkit_dom_test_nondeterministic_class_init(WebKitDOMTestNondeterministicClass* requestClass)
{
    GObjectClass* gobjectClass = G_OBJECT_CLASS(requestClass);
    g_type_class_add_private(gobjectClass, sizeof(WebKitDOMTestNondeterministicPrivate));
    gobjectClass->constructor = webkit_dom_test_nondeterministic_constructor;
    gobjectClass->finalize = webkit_dom_test_nondeterministic_finalize;
    gobjectClass->set_property = webkit_dom_test_nondeterministic_set_property;
    gobjectClass->get_property = webkit_dom_test_nondeterministic_get_property;

    g_object_class_install_property(
        gobjectClass,
        PROP_NONDETERMINISTIC_READONLY_ATTR,
        g_param_spec_long(
            "nondeterministic-readonly-attr",
            "TestNondeterministic:nondeterministic-readonly-attr",
            "read-only glong TestNondeterministic:nondeterministic-readonly-attr",
            G_MINLONG, G_MAXLONG, 0,
            WEBKIT_PARAM_READABLE));

    g_object_class_install_property(
        gobjectClass,
        PROP_NONDETERMINISTIC_WRITEABLE_ATTR,
        g_param_spec_string(
            "nondeterministic-writeable-attr",
            "TestNondeterministic:nondeterministic-writeable-attr",
            "read-write gchar* TestNondeterministic:nondeterministic-writeable-attr",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_NONDETERMINISTIC_EXCEPTION_ATTR,
        g_param_spec_string(
            "nondeterministic-exception-attr",
            "TestNondeterministic:nondeterministic-exception-attr",
            "read-write gchar* TestNondeterministic:nondeterministic-exception-attr",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_NONDETERMINISTIC_GETTER_EXCEPTION_ATTR,
        g_param_spec_string(
            "nondeterministic-getter-exception-attr",
            "TestNondeterministic:nondeterministic-getter-exception-attr",
            "read-write gchar* TestNondeterministic:nondeterministic-getter-exception-attr",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_NONDETERMINISTIC_SETTER_EXCEPTION_ATTR,
        g_param_spec_string(
            "nondeterministic-setter-exception-attr",
            "TestNondeterministic:nondeterministic-setter-exception-attr",
            "read-write gchar* TestNondeterministic:nondeterministic-setter-exception-attr",
            "",
            WEBKIT_PARAM_READWRITE));

}

static void webkit_dom_test_nondeterministic_init(WebKitDOMTestNondeterministic* request)
{
    WebKitDOMTestNondeterministicPrivate* priv = WEBKIT_DOM_TEST_NONDETERMINISTIC_GET_PRIVATE(request);
    new (priv) WebKitDOMTestNondeterministicPrivate();
}

gboolean webkit_dom_test_nondeterministic_nondeterministic_zero_arg_function(WebKitDOMTestNondeterministic* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_TEST_NONDETERMINISTIC(self), FALSE);
    WebCore::TestNondeterministic* item = WebKit::core(self);
    gboolean result = item->nondeterministicZeroArgFunction();
    return result;
}

glong webkit_dom_test_nondeterministic_get_nondeterministic_readonly_attr(WebKitDOMTestNondeterministic* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_TEST_NONDETERMINISTIC(self), 0);
    WebCore::TestNondeterministic* item = WebKit::core(self);
    glong result = item->nondeterministicReadonlyAttr();
    return result;
}

gchar* webkit_dom_test_nondeterministic_get_nondeterministic_writeable_attr(WebKitDOMTestNondeterministic* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_TEST_NONDETERMINISTIC(self), 0);
    WebCore::TestNondeterministic* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->nondeterministicWriteableAttr());
    return result;
}

void webkit_dom_test_nondeterministic_set_nondeterministic_writeable_attr(WebKitDOMTestNondeterministic* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_TEST_NONDETERMINISTIC(self));
    g_return_if_fail(value);
    WebCore::TestNondeterministic* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setNondeterministicWriteableAttr(convertedValue);
}

gchar* webkit_dom_test_nondeterministic_get_nondeterministic_exception_attr(WebKitDOMTestNondeterministic* self, GError** error)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_TEST_NONDETERMINISTIC(self), 0);
    g_return_val_if_fail(!error || !*error, 0);
    WebCore::TestNondeterministic* item = WebKit::core(self);
    WebCore::ExceptionCode ec = 0;
    gchar* result = convertToUTF8String(item->nondeterministicExceptionAttr(ec));
    return result;
}

void webkit_dom_test_nondeterministic_set_nondeterministic_exception_attr(WebKitDOMTestNondeterministic* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_TEST_NONDETERMINISTIC(self));
    g_return_if_fail(value);
    WebCore::TestNondeterministic* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setNondeterministicExceptionAttr(convertedValue);
}

gchar* webkit_dom_test_nondeterministic_get_nondeterministic_getter_exception_attr(WebKitDOMTestNondeterministic* self, GError** error)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_TEST_NONDETERMINISTIC(self), 0);
    g_return_val_if_fail(!error || !*error, 0);
    WebCore::TestNondeterministic* item = WebKit::core(self);
    WebCore::ExceptionCode ec = 0;
    gchar* result = convertToUTF8String(item->nondeterministicGetterExceptionAttr(ec));
    return result;
}

void webkit_dom_test_nondeterministic_set_nondeterministic_getter_exception_attr(WebKitDOMTestNondeterministic* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_TEST_NONDETERMINISTIC(self));
    g_return_if_fail(value);
    WebCore::TestNondeterministic* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setNondeterministicGetterExceptionAttr(convertedValue);
}

gchar* webkit_dom_test_nondeterministic_get_nondeterministic_setter_exception_attr(WebKitDOMTestNondeterministic* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_TEST_NONDETERMINISTIC(self), 0);
    WebCore::TestNondeterministic* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->nondeterministicSetterExceptionAttr());
    return result;
}

void webkit_dom_test_nondeterministic_set_nondeterministic_setter_exception_attr(WebKitDOMTestNondeterministic* self, const gchar* value, GError** error)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_TEST_NONDETERMINISTIC(self));
    g_return_if_fail(value);
    g_return_if_fail(!error || !*error);
    WebCore::TestNondeterministic* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    WebCore::ExceptionCode ec = 0;
    item->setNondeterministicSetterExceptionAttr(convertedValue, ec);
    if (ec) {
        WebCore::ExceptionCodeDescription ecdesc(ec);
        g_set_error_literal(error, g_quark_from_string("WEBKIT_DOM"), ecdesc.code, ecdesc.name);
    }
}


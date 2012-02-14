/*
 ============================================================================
 Name        : hev-impathy.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_IMPATHY_H__
#define __HEV_IMPATHY_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define HEV_TYPE_IMPATHY	(hev_impathy_get_type())
#define HEV_IMPATHY(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_IMPATHY, HevImpathy))
#define HEV_IS_IMPATHY(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_IMPATHY))
#define HEV_IMPATHY_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_IMPATHY, HevImpathyClass))
#define HEV_IS_IMPATHY_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_IMPATHY))
#define HEV_IMPATHY_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_IMPATHY, HevImpathyClass))

typedef struct _HevImpathy HevImpathy;
typedef struct _HevImpathyClass HevImpathyClass;

struct _HevImpathy
{
	GObject parent_instance;
};

struct _HevImpathyClass
{
	GObjectClass parent_class;
};

GType hev_impathy_get_type(void);

GObject * hev_impathy_new(void);

GtkWidget * hev_impathy_get_plug(HevImpathy *self,
			GdkNativeWindow socket_id);

G_END_DECLS

#endif /* __HEV_IMPATHY_H__ */


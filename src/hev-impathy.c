/*
 ============================================================================
 Name        : hev-impathy.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include <telepathy-glib/telepathy-glib.h>
#include <telepathy-glib/account-manager.h>
#include <telepathy-glib/account.h>

#include "hev-impathy.h"

#define HEV_IMPATHY_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_IMPATHY, HevImpathyPrivate))

typedef struct _HevImpathyPrivate HevImpathyPrivate;

struct _HevImpathyPrivate
{
	GtkWidget *plug;
	GtkWidget *combo_box_text_accounts;
	GtkWidget *combo_box_text_presences;
	GtkWidget *notebook_contacts;
	GtkWidget *label_wall;
	GtkListStore *list_store_contacts;
	TpAccountManager *account_manager;
};

G_DEFINE_TYPE(HevImpathy, hev_impathy, G_TYPE_OBJECT);

static void hev_impathy_dispose(GObject * obj)
{
	HevImpathy * self = HEV_IMPATHY(obj);
	HevImpathyPrivate * priv = HEV_IMPATHY_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->plug)
	{
		g_object_unref(priv->plug);
		priv->plug = NULL;
	}

	if(priv->account_manager)
	{
		g_object_unref(priv->account_manager);
		priv->account_manager = NULL;
	}

	G_OBJECT_CLASS(hev_impathy_parent_class)->dispose(obj);
}

static void hev_impathy_finalize(GObject * obj)
{
	HevImpathy * self = HEV_IMPATHY(obj);
	HevImpathyPrivate * priv = HEV_IMPATHY_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_impathy_parent_class)->finalize(obj);
}

static GObject * hev_impathy_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_impathy_parent_class)->constructor(type, n, param);
}

static void hev_impathy_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_impathy_parent_class)->constructed(obj);
}

static void hev_impathy_class_init(HevImpathyClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_impathy_constructor;
	obj_class->constructed = hev_impathy_constructed;
	obj_class->dispose = hev_impathy_dispose;
	obj_class->finalize = hev_impathy_finalize;

	g_type_class_add_private(klass, sizeof(HevImpathyPrivate));
}

static void hev_impathy_init(HevImpathy * self)
{
	HevImpathyPrivate * priv = HEV_IMPATHY_GET_PRIVATE(self);
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

GObject * hev_impathy_new(void)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return g_object_new(HEV_TYPE_IMPATHY, NULL);
}

static void account_manager_prepare_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	HevImpathy *self = HEV_IMPATHY(user_data);
	HevImpathyPrivate *priv = HEV_IMPATHY_GET_PRIVATE(self);
	GError *error = NULL;
	GList *account_list = NULL, *acct = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(!tp_proxy_prepare_finish(TP_PROXY(source_object),
					res, &error))
	{
		g_debug("%s:%d[%s]=>(%s)", __FILE__, __LINE__,
					__FUNCTION__, error->message);
		g_error_free(error);
		return;
	}

	account_list = tp_account_manager_get_valid_accounts(
				TP_ACCOUNT_MANAGER(source_object));
	for(acct=g_list_first(account_list); acct; acct=g_list_next(acct))
	{
		TpAccount *account = TP_ACCOUNT(acct->data);
		TpConnection *connection = tp_account_get_connection(account);
		GPtrArray *contacts = NULL;
		guint i = 0;

		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(priv->combo_box_text_accounts),
					tp_account_get_display_name(TP_ACCOUNT(account)));

		if((NULL==connection) || (TP_CONTACT_LIST_STATE_SUCCESS!=
					tp_connection_get_contact_list_state(connection)))
		  continue;

		contacts = tp_connection_dup_contact_list(connection);
		for(i=0; i<contacts->len; i++)
		{
			TpContact *contact = g_ptr_array_index(contacts, i);
			GtkTreeIter tree_iter = { 0 };

			gtk_list_store_append(priv->list_store_contacts,
						&tree_iter);
			gtk_list_store_set(priv->list_store_contacts,
						&tree_iter, 0, tp_contact_get_alias(contact),
						-1);
		}
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(priv->combo_box_text_accounts), 0);

	g_list_free(account_list);
}

static void accounts_combo_box_changed_handler(GtkComboBox *widget,
			gpointer user_data)
{
	HevImpathy *self = HEV_IMPATHY(user_data);
	HevImpathyPrivate *priv = HEV_IMPATHY_GET_PRIVATE(self);
	GList *account_list = NULL, *acct = NULL;
	gint i = 0, pos = 0;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	pos = gtk_combo_box_get_active(widget);
	account_list = tp_account_manager_get_valid_accounts(
				TP_ACCOUNT_MANAGER(priv->account_manager));
	for(acct=g_list_first(account_list),i=0; acct; acct=g_list_next(acct),i++)
	{
		TpAccount *account = TP_ACCOUNT(acct->data);

		if(i == pos)
		{
			TpConnectionPresenceType type = TP_CONNECTION_PRESENCE_TYPE_UNSET;
			gchar *status = NULL, *message = NULL;

			type = tp_account_get_current_presence(account,
						&status, &message);
			switch(type)
			{
			case TP_CONNECTION_PRESENCE_TYPE_AVAILABLE:
				gtk_combo_box_set_active(GTK_COMBO_BOX(priv->combo_box_text_presences),
							0);
				break;
			case TP_CONNECTION_PRESENCE_TYPE_AWAY:
				gtk_combo_box_set_active(GTK_COMBO_BOX(priv->combo_box_text_presences),
							1);
				break;
			case TP_CONNECTION_PRESENCE_TYPE_OFFLINE:
				gtk_combo_box_set_active(GTK_COMBO_BOX(priv->combo_box_text_presences),
							2);
				break;
			}

			break;
		}
	}
}

static GtkWidget * hev_impathy_ui_new(HevImpathy *self)
{
	HevImpathyPrivate *priv = HEV_IMPATHY_GET_PRIVATE(self);
	GtkWidget *vbox = NULL;
	GtkWidget *hbox = NULL;
	GtkWidget *label = NULL;
	GtkWidget *scrolled_window = NULL;
	GtkWidget *tree_view = NULL;
	gchar *str = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox),
				hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	label = gtk_label_new("Accounts: ");
	gtk_box_pack_start(GTK_BOX(hbox),
				label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	priv->combo_box_text_accounts =
		gtk_combo_box_text_new();
	g_signal_connect(G_OBJECT(priv->combo_box_text_accounts),
				"changed", G_CALLBACK(accounts_combo_box_changed_handler),
				self);
	gtk_box_pack_start(GTK_BOX(hbox),
				priv->combo_box_text_accounts,
				FALSE, FALSE, 0);
	gtk_widget_show(priv->combo_box_text_accounts);

	label = gtk_label_new("Presences: ");
	gtk_box_pack_start(GTK_BOX(hbox),
				label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	priv->combo_box_text_presences =
		gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(
				GTK_COMBO_BOX_TEXT(priv->combo_box_text_presences),
				"Available");
	gtk_combo_box_text_append_text(
				GTK_COMBO_BOX_TEXT(priv->combo_box_text_presences),
				"Away");
	gtk_combo_box_text_append_text(
				GTK_COMBO_BOX_TEXT(priv->combo_box_text_presences),
				"Offline");
	gtk_box_pack_start(GTK_BOX(hbox),
				priv->combo_box_text_presences,
				FALSE, FALSE, 0);
	gtk_widget_show(priv->combo_box_text_presences);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox),
				hbox, TRUE, TRUE, 0);
	gtk_widget_show(hbox);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(hbox), scrolled_window,
				FALSE, TRUE, 0);
	gtk_widget_show(scrolled_window);

	priv->notebook_contacts = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(hbox), priv->notebook_contacts,
				TRUE, TRUE, 0);
	//gtk_widget_show(priv->notebook_contacts);
	
	priv->label_wall = gtk_label_new(NULL);
	str = g_markup_printf_escaped("<span size=\"larger\"><b>LOPhone</b></span>");
	gtk_label_set_markup(GTK_LABEL(priv->label_wall),
				str);
	g_free(str);
	gtk_box_pack_start(GTK_BOX(hbox), priv->label_wall,
				TRUE, TRUE, 0);
	gtk_widget_show(priv->label_wall);

	tree_view = gtk_tree_view_new();
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree_view),
				FALSE);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view),
				-1, "Name", gtk_cell_renderer_text_new(), "text", 0,
				NULL);
	gtk_widget_set_size_request(tree_view, 140, -1);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),
				tree_view);
	gtk_widget_show(tree_view);

	priv->list_store_contacts = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view),
				GTK_TREE_MODEL(priv->list_store_contacts));

	/* account_manager prepare async */
	{
		TpSimpleClientFactory *factory = NULL;

		priv->account_manager = tp_account_manager_dup();
		factory = tp_proxy_get_factory(priv->account_manager);

		tp_simple_client_factory_add_account_features_varargs(factory,
					TP_ACCOUNT_FEATURE_CONNECTION, 0);
		tp_simple_client_factory_add_connection_features_varargs(factory,
					TP_CONNECTION_FEATURE_CONTACT_LIST, 0);
		tp_simple_client_factory_add_contact_features_varargs(factory,
					TP_CONTACT_FEATURE_ALIAS, TP_CONTACT_FEATURE_CONTACT_GROUPS,
					TP_CONTACT_FEATURE_INVALID);
		tp_proxy_prepare_async(TP_PROXY(priv->account_manager), NULL,
					account_manager_prepare_async_handler, self);
	}

	return vbox;
}

GtkWidget * hev_impathy_get_plug(HevImpathy *self,
			GdkNativeWindow socket_id)
{
	HevImpathyPrivate *priv = NULL;
	GtkWidget *plug = NULL;
	GtkWidget *vbox = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_IMPATHY(self), NULL);
	priv = HEV_IMPATHY_GET_PRIVATE(self);

	if(priv->plug)
	{
		g_object_unref(priv->plug);
		priv->plug = NULL;
	}

	plug = gtk_plug_new(socket_id);
	priv->plug = g_object_ref(plug);

	vbox = hev_impathy_ui_new(self);
	gtk_container_add(GTK_CONTAINER(priv->plug),
				vbox);

	return priv->plug;
}


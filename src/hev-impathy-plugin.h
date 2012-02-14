/*
 ============================================================================
 Name        : hev-impathy-plugin.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_IMPATHY_PLUGIN_H__
#define __HEV_IMPATHY_PLUGIN_H__

NP_EXPORT(NPError) NP_Initialize(NPNetscapeFuncs *npn_funcs, NPPluginFuncs *npp_funcs);
NP_EXPORT(NPError) NP_Shutdown(void);
NP_EXPORT(const char *) NP_GetMIMEDescription(void);
NP_EXPORT(NPError) NP_GetValue(void *future, NPPVariable variable, void *value);

#endif /* __HEV_IMPATHY_PLUGIN_H__ */


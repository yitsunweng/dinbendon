/**************************************************************************
 *
 * GEMTEK CONFIDENTIAL
 *
 * Copyright 2006-2007 Gemtek Technology Co.,Ltd. All Rights Reserved.
 *
 * The source code contained or described herein and all documents related
 * to the source code ("Material") are owned by Gemtek Technology Co.,Ltd.
 * or its suppliers or licensors. Title to the Material remains with
 * Gemtek Technology Co.,Ltd. or its suppliers and licensors. The Material
 * contains trade secrets and proprietary and confidential information of
 * Gemtek or its suppliers and licensors. The Material is protected by
 * worldwide copyright and trade secret laws and treaty provisions.
 * No part of the Material may be used, copied, reproduced, modified,
 * published, uploaded, posted, transmitted, distributed, or disclosed
 * in any way without Gemtek's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you by
 * disclosure or delivery of the Materials, either expressly, by
 * implication, inducement, estoppel or otherwise. Any license under such
 * intellectual property rights must be express and approved by Gemtek in
 * writing.
 *
 **************************************************************************/

#ifndef _LIBCGI_H
#define _LIBCGI_H

#include <stdio.h>

/* defined in libcgi.c */
char *get_query(FILE *stream);
char *get_query2(FILE *stream,char **);
void set_cgi(char *name, char *value);
char *get_cgi(char *name);
void init_cgi(char *query);
/* libcgi.c */

/* defined in webs.c */
void webs_redirect(char *site, char *page, int timeout, FILE *stream);
void webs_write(char *page, FILE *stream);
void webs_setCookie(char *name, char *value, char *page, FILE *stream);
/* webs.c */

#endif // _LIBCGI_H

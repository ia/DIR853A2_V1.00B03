/*
 * form.c -- Form processing (in-memory CGI) for the GoAhead Web server
 *
 * Copyright (c) GoAhead Software Inc., 1995-2010. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 * 
 */

/********************************** Description *******************************/

/*
 *	This module implements the /goform handler. It emulates CGI processing
 *	but performs this in-process and not as an external process. This enables
 *	a very high performance implementation with easy parsing and decoding 
 *	of query strings and posted data.
 */

/*********************************** Includes *********************************/

#include	"wsIntrn.h"

/************************************ Locals **********************************/

static sym_fd_t	formSymtab = -1;			/* Symbol table for form handlers */

/************************************* Code ***********************************/
/*
 *	Process a form request. Returns 1 always to indicate it handled the URL
 */

int websFormHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
	char_t *url, char_t *path, char_t *query)
{
	sym_t		*sp;
	char_t		formBuf[FNAMESIZE];
	char_t		*cp, *formName;
	int			(*fn)(void *sock, char_t *path, char_t *args);

	a_assert(websValid(wp));
	a_assert(url && *url);
	a_assert(path && *path == '/');

	websStats.formHits++;

/*
 *	Extract the form name
 */
 	//printf("websFormHandler path = %s\n", path);//carlos debug
	gstrncpy(formBuf, path, TSZ(formBuf));
	if ((formName = gstrchr(&formBuf[1], '/')) == NULL) {
		websError(wp, 200, T("Missing form name"));
		return 1;
	}
	formName++;
	if ((cp = gstrchr(formName, '/')) != NULL) {
		*cp = '\0';
	}
	//printf("Form name = %s , wp->query = %s\n", formName, wp->query);//carlos debug
/*
 *	Lookup the C form function first and then try tcl (no javascript support 
 *	yet).
 */
	sp = symLookup(formSymtab, formName);
	if (sp == NULL) {
		websError(wp, 404, T("Form %s is not defined"), formName);
	} else {
		fn = (int (*)(void *, char_t *, char_t *)) sp->content.value.integer;
		a_assert(fn);
		if (fn) {
/*
 *			For good practice, forms must call websDone()
 */
			(*fn)((void*) wp, formName, query);

/*
 *			Remove the test to force websDone, since this prevents
 *			the server "push" from a form>
 */
#if 0 /* push */
			if (websValid(wp)) {
				websError(wp, 200, T("Form didn't call websDone"));
			}
#endif /* push */
		}
	}
	return 1;
}

/******************************************************************************/
/*
 *	Define a form function in the "form" map space.
 */

int websFormDefine(char_t *name, void (*fn)(webs_t wp, char_t *path, 
	char_t *query))
{
	a_assert(name && *name);
	a_assert(fn);

	if (fn == NULL) {
		return -1;
	}

	symEnter(formSymtab, name, valueInteger((int) fn), (int) NULL);
	return 0;
}

/******************************************************************************/
/*
 *	Open the symbol table for forms.
 */

void websFormOpen()
{
	formSymtab = symOpen(WEBS_SYM_INIT);
}

/******************************************************************************/
/*
 *	Close the symbol table for forms.
 */

void websFormClose()
{
	if (formSymtab != -1) {
		symClose(formSymtab);
		formSymtab = -1;
	}
}

/******************************************************************************/
/*
 *	Write a webs header. This is a convenience routine to write a common
 *	header for a form back to the browser.
 */

void websHeader(webs_t wp)
{
	a_assert(websValid(wp));

	websWrite(wp, T("HTTP/1.0 200 OK\n"));

/*
 *	The Server HTTP header below must not be modified unless
 *	explicitly allowed by licensing terms.
 */
#ifdef WEBS_SSL_SUPPORT
	websWrite(wp, T("Server: %s/%s %s/%s\r\n"), 
		WEBS_NAME, WEBS_VERSION, SSL_NAME, SSL_VERSION);
#else
	websWrite(wp, T("Server: %s/%s\r\n"), WEBS_NAME, WEBS_VERSION);
#endif

	websWrite(wp, T("Pragma: no-cache\n"));
	websWrite(wp, T("Cache-control: no-cache\n"));
	websWrite(wp, T("Content-Type: text/html\n"));
	websWrite(wp, T("\n"));
	websWrite(wp, T("<html>\n"));
}

/* DLINK add/s [Carlos: 2014/01/02]*/
void websHeader2(webs_t wp)
{
    a_assert(websValid(wp));

    websWrite(wp, T("HTTP/1.0 200 OK\n"));

/*
 *  The Server HTTP header below must not be modified unless
 *  explicitly allowed by licensing terms.
 */
#ifdef WEBS_SSL_SUPPORT
    websWrite(wp, T("Server: %s/%s %s/%s\r\n"),
        WEBS_NAME, WEBS_VERSION, SSL_NAME, SSL_VERSION);
#else
    websWrite(wp, T("Server: %s/%s\r\n"), WEBS_NAME, WEBS_VERSION);
#endif

    websWrite(wp, T("Pragma: no-cache\n"));
    websWrite(wp, T("Cache-control: no-cache\n"));
    websWrite(wp, T("Content-Type: text/html\n"));
    websWrite(wp, T("\n"));
//    websWrite(wp, T("<html>\n"));
}

/* DLINK add/s [James: 2014/05/09: changed 'Content-Type: text/html' to 'Content-Type: text/xml']*/
void websXmlHeader(webs_t wp)
{
    a_assert(websValid(wp));

    websWrite(wp, T("HTTP/1.0 200 OK\n"));

/*
 *  The Server HTTP header below must not be modified unless
 *  explicitly allowed by licensing terms.
 */
#ifdef WEBS_SSL_SUPPORT
    websWrite(wp, T("Server: %s/%s %s/%s\r\n"),
        WEBS_NAME, WEBS_VERSION, SSL_NAME, SSL_VERSION);
#else
    websWrite(wp, T("Server: %s/%s\r\n"), WEBS_NAME, WEBS_VERSION);
#endif

    websWrite(wp, T("Pragma: no-cache\n"));
    websWrite(wp, T("Cache-control: no-cache\n"));
    websWrite(wp, T("Content-Type: text/xml\n"));
    websWrite(wp, T("\n"));
//    websWrite(wp, T("<html>\n"));
}
/* DLINK add/e */

/******************************************************************************/
/*
 *	Write a webs footer
 */

void websFooter(webs_t wp)
{
	a_assert(websValid(wp));

	websWrite(wp, T("</html>\n"));
}

/******************************************************************************/


#ifndef _GETMSGTEXT_H_
#define _GETMSGTEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

bool LoadLanguage(const char* langfile);
const char *gettextmsg(const char *msg);

#ifdef __cplusplus
}
#endif

#endif /* _GETTEXT_H_ */

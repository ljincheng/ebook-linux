
#ifndef __GEPUB_NAV_H__
#define __GEPUB_NAV_H__

typedef struct _GepubNavPoint  GepubNavPoint;
struct _GepubNavPoint
{
   gchar    *text;
   gchar       * id;
  gchar        *playOrder;
   gchar       *src;
  gboolean    hasNext;
  gboolean    hasChildren;
  GepubNavPoint *children;
  GepubNavPoint *next;
};

#endif /* __GEPUB_NAV_H__ */

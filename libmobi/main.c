
#include <mobi.h>
#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>
#define ERROR 0
#define SUCCESS 1

int chapter_index=0;

static void
destroy_cb (GtkWidget *window,
            BookWidget *view)
{

    BookDoc *bookDoc;
  bookDoc= book_widget_get_doc(view);
  book_doc_free (bookDoc);
    gtk_main_quit ();
}


static gboolean  window_key_press(GtkWidget * widget, GdkEventKey  *event,BookWidget *view )
{
  switch (event->keyval) {
	case GDK_KEY_Right:
	case GDK_KEY_j:
	case GDK_KEY_space:
	{
    book_widget_chapter_next(view);

	}
	break;

	case GDK_KEY_Left:
	case GDK_KEY_k:
	{
    book_widget_chapter_prev(view);
	}
	break;

	case GDK_KEY_Page_Up:
	{
	}
	break;

	case GDK_KEY_Page_Down:
	{
	}
	break;

	//　以下设置显示主题
	case GDK_KEY_1:
	{
	}
	break;

	case GDK_KEY_2:
	{
	}
	break;

	case GDK_KEY_3:
	{
	}
	break;
  case GDK_KEY_8:
      {

      }
      break;
    case GDK_KEY_9:
      {

      }
      break;
// 以下设置文件内容

	case GDK_KEY_F1:
	{
		break;
	}
	case GDK_KEY_F2:
	{
   	return FALSE;
	}
	break;

	case GDK_KEY_F3:
	{

	}
	break;
  case GDK_KEY_F5:
      {
              g_print("F5:\n");
      }
                           break;
  case GDK_KEY_Escape:
      {
   	return TRUE;
      }
break;
	default:
		g_print("key press GDK_%x\n", event->keyval);
		break;
	}

	return FALSE;
}

int main_bk(int argc, char **argv)
{
    GtkWidget *window;
    GtkWidget *widget;
     gtk_init (&argc, &argv);

    widget = book_widget_new ();
   webkit_settings_set_enable_developer_extras (
        webkit_web_view_get_settings (WEBKIT_WEB_VIEW (widget)), TRUE);

   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window), 1200, 800);
    gtk_container_add (GTK_CONTAINER (window), widget);

gtk_widget_show_all (window);


/* Initialize main MOBIData structure */
/* Must be deallocated with mobi_free() when not needed */
MOBIData *m = mobi_init();
if (m == NULL) {
  return ERROR;
}


/* Open file for reading */
FILE *file = fopen("/workspace/temp/ebook/test.mobi", "rb");
if (file == NULL) {
  mobi_free(m);
  return ERROR;
}

/* Load file into MOBIData structure */
/* This structure will hold raw data/metadata from mobi document */
MOBI_RET mobi_ret = mobi_load_file(m, file);
fclose(file);
if (mobi_ret != MOBI_SUCCESS) {
  mobi_free(m);
  return ERROR;
}


/* Initialize MOBIRawml structure */
/* Must be deallocated with mobi_free_rawml() when not needed */
/* In the next step this structure will be filled with parsed data */
MOBIRawml *rawml = mobi_init_rawml(m);
if (rawml == NULL) {
  mobi_free(m);
  return ERROR;
}

/*
  FILE *rawmlfile = fopen("/workspace/temp/test-1.rawml", "wb");
  mobi_dump_rawml(m,rawmlfile);
  fclose(rawmlfile);
*/



/* Raw data from MOBIData will be converted to html, css, fonts, media resources */
/* Parsed data will be available in MOBIRawml structure */
mobi_ret = mobi_parse_rawml(rawml, m);
if (mobi_ret != MOBI_SUCCESS) {
  mobi_free(m);
  mobi_free_rawml(rawml);
  return ERROR;
}



   if (rawml->markup != NULL) {
        /* Linked list of MOBIPart structures in rawml->markup holds main text files */
        MOBIPart *curr = rawml->markup;
int index=0;
    char partname[200];
        while (curr != NULL) {
            if (curr->size > 0) {
            MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
            //snprintf(partname, sizeof(partname), "OEBPS/part%05zu.%s", curr->uid, file_meta.extension);
            //mz_ret = mz_zip_writer_add_mem(&zip, partname, curr->data, curr->size, (mz_uint) MZ_DEFAULT_COMPRESSION);
// printf("内容：%05zu.%s,%s\n\r\n",curr->uid, file_meta.extension,curr->data);
             // printf("内容：%d:: %05zu.%s\n",index,curr->uid, file_meta.extension);

                     snprintf(partname, sizeof(partname), "/workspace/temp/mobires/markup/resource%05zu.%s",  curr->uid, file_meta.extension);

               //printf("文件：%s\n",partname);
                FILE *resFile = fopen(partname, "wb");
               fwrite(curr->data, 1, curr->size, resFile);
               fclose(file);
                                  if(index==3)
            book_widget_set_html(widget,g_bytes_new_take (curr->data, curr->size));
          index++;
                                   }
            curr = curr->next;
        }
    }

   if (rawml->flow != NULL) {
        /* Linked list of MOBIPart structures in rawml->markup holds main text files */
        MOBIPart *curr = rawml->flow;
int index=0;
    char partname[200];
       curr = curr->next;
        while (curr != NULL) {
            if (curr->size > 0) {
            MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
            //snprintf(partname, sizeof(partname), "OEBPS/part%05zu.%s", curr->uid, file_meta.extension);
            //mz_ret = mz_zip_writer_add_mem(&zip, partname, curr->data, curr->size, (mz_uint) MZ_DEFAULT_COMPRESSION);
// printf("内容：%05zu.%s,%s\n\r\n",curr->uid, file_meta.extension,curr->data);
             // printf("内容：%d:: %05zu.%s\n",index,curr->uid, file_meta.extension);

                     snprintf(partname, sizeof(partname), "/workspace/temp/mobires/flow/resource%05zu.%s",  curr->uid, file_meta.extension);

                FILE *resFile = fopen(partname, "wb");
               fwrite(curr->data, 1, curr->size, resFile);
               fclose(file);
          index++;
                                   }
            curr = curr->next;
        }
    }


  if (rawml->resources != NULL) {
        /* Linked list of MOBIPart structures in rawml->markup holds main text files */
        MOBIPart *curr = rawml->resources;
int index=0;
    char partname[200];

        while (curr != NULL) {
          MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
            if (curr->size > 0) {
            //snprintf(partname, sizeof(partname), "OEBPS/part%05zu.%s", curr->uid, file_meta.extension);
            //mz_ret = mz_zip_writer_add_mem(&zip, partname, curr->data, curr->size, (mz_uint) MZ_DEFAULT_COMPRESSION);
// printf("内容：%05zu.%s,%s\n\r\n",curr->uid, file_meta.extension,curr->data);
             // printf("内容：%d:: %05zu.%s\n",index,curr->uid, file_meta.extension);
             //  printf("文件type[%d]：%d ,%05zu \n",index,file_meta.type, curr->uid);
                if (file_meta.type == T_OPF) {
                    snprintf(partname, sizeof(partname), "/workspace/temp/mobires/opf/content.opf");
                  }else{
                     snprintf(partname, sizeof(partname), "/workspace/temp/mobires/img/resource%05zu.%s",  curr->uid, file_meta.extension);
                                                    }

              // printf("文件：%s\n",partname);
                FILE *resFile = fopen(partname, "wb");
               fwrite(curr->data, 1, curr->size, resFile);
               fclose(file);
              index++;
                                   }
            curr = curr->next;
        }
    }

   gtk_main ();

/* Do something useful here */
/* ... */
/* For examples how to access data in MOBIRawml structure see mobitool.c */

/* Free MOBIRawml structure */
mobi_free_rawml(rawml);

/* Free MOBIData structure */
mobi_free(m);


return SUCCESS;
}


int main(int argc, char **argv)
{
    GtkWidget *window;
    GtkWidget *widget;
    BookDoc *bookDoc;
     gtk_init (&argc, &argv);

    widget = book_widget_new ();
   webkit_settings_set_enable_developer_extras (
        webkit_web_view_get_settings (WEBKIT_WEB_VIEW (widget)), TRUE);

   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window), 1200, 800);
    gtk_container_add (GTK_CONTAINER (window), widget);
g_signal_connect (window, "destroy", G_CALLBACK(destroy_cb), widget);
  g_signal_connect(window, "key-press-event", G_CALLBACK(window_key_press), widget);


 // bookDoc= book_doc_init ("/workspace/temp/ebook/加密与解密-第三版.epub");
 //  bookDoc= book_doc_init ("/workspace/temp/ebook/3D打印：从想象到现实.mobi");
 // bookDoc= book_doc_init ("/workspace/temp/ebook/加密与解密（第3版）- 段钢.azw3");
  bookDoc= book_doc_init ("/workspace/temp/ebook/huffdic.mobi");
  book_widget_set_doc (widget,bookDoc);
  gtk_window_set_title(window, bookDoc->bookName);
  gtk_widget_show_all (window);


   gtk_main ();

  book_doc_free(bookDoc);


return SUCCESS;
}

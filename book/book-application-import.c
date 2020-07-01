#include "book-application-import.h"
#include "book-manage.h"
#include "book-doc.h"
#include "book-pdf.h"
#include "book-dialogs.h"

void book_application_import_bookinfo(const gchar * dirpath)
{
	GDir *dir;
	const gchar *name;
   char *ext;
	GError                 *error = NULL;
	dir = g_dir_open(dirpath, 0, &error);
  if (error) {
						      book_log_debug("打开目录失败：%s\n", error->message);
      g_error_free(error);
                error=NULL;
    return ;
  }
	//if (!dir)
	//	return;


	name = g_dir_read_name(dir);
	while ((	name = g_dir_read_name(dir)) != NULL) {
		gchar *path, *display_name;
		gboolean is_dir;
		if (name[0] != '.') {
			path = g_build_filename(dirpath, name, NULL);
			is_dir = g_file_test(path, G_FILE_TEST_IS_DIR);
			display_name = g_filename_to_utf8(name, -1, NULL, NULL, NULL);
			if (!is_dir ) {
        ext= strrchr(path,'.');
				if (ext!=NULL && g_file_test (path, G_FILE_TEST_IS_REGULAR) && (strcmp(ext, ".epub")==0 || strcmp(ext, ".mobi")==0 || strcmp(ext, ".azw3")==0)) {
					    BookDoc * bookDoc = book_doc_new(path,  &error);
					    if (error) {
						      g_printf("打开书籍失败：%s\n", error->message);
						      g_error_free(error);
                error=NULL;
					    }else if (bookDoc != NULL ) {
						          gchar *bookId = book_doc_get_metadata(bookDoc, BOOK_META_ID);
						          if (bookId != NULL) {
							              gchar * bookTitle = book_doc_get_metadata(bookDoc, BOOK_META_TITLE);
							              gchar *bookAuthor = book_doc_get_metadata(bookDoc, BOOK_META_AUTHOR);
							             // g_printf("bookId=%s,bookTitle=%s,bookAuthor=%s\n", bookId,bookTitle,bookAuthor);
					              		//insertBookInfo(bookId, bookTitle, bookAuthor, path);
                        book_manage_insert_bookmeta (bookId, bookTitle, bookAuthor, path,NULL,NULL);
							              g_free(bookId);
							              g_free(bookAuthor);
							              g_free(bookTitle);
						                      }
				            	}
  if(bookDoc!=NULL)
        {
           g_clear_object(&bookDoc);
        }
		  	}

			} else {
		  		book_application_import_bookinfo(path);
			}
			g_free(path);
			g_free(display_name);
		}
  //g_printf("结束：name=%s\n",name);
	}
	g_dir_close(dir);
}

void book_application_import_folder(char *filename)
{
   gtk_spinner_start (GTK_SPINNER (APPAPINNER));
  book_application_import_bookinfo(filename);
 //book_database_test_import (filename);
   gtk_spinner_stop (GTK_SPINNER (APPAPINNER));
}

void book_application_import_open_folder_response_cb (GtkNativeDialog *dialog,
                  gint             response_id,
                  gpointer         user_data)
{
  GtkFileChooserNative *native = user_data;
  GApplication *app = g_object_get_data (G_OBJECT (native), "app");
  GtkWidget *message_dialog;
  GError *error = NULL;

  if (response_id == GTK_RESPONSE_ACCEPT)
    {
        char *filename,*dirPath;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);
    filename = gtk_file_chooser_get_current_folder (chooser);
      g_printf("打开的目录：%s\n",filename);

    //gdk_threads_init();

       g_thread_create(book_application_import_folder,filename,FALSE,NULL);
	//book_database_import_bookinfo(filename);
	//gdk_threads_enter();
      printf("完成导入\n");


//g_free (filename);
    }

  gtk_native_dialog_destroy (GTK_NATIVE_DIALOG (native));
  g_object_unref (native);
}


void book_application_import_open_response_cb (GtkNativeDialog *dialog,
                  gint             response_id,
                  gpointer         user_data)
{
  GtkFileChooserNative *native = user_data;
  GApplication *app = g_object_get_data (G_OBJECT (native), "app");
  GtkWidget *message_dialog;
  GError *error = NULL;
  gchar *filePath,*fileName,*fileExt;
 GFile * selectFile;
  gchar *bookTitle,*bookAuthor,*bookCreateTime;
  gchar *md5,*inputPwd=NULL;
bookTitle=NULL;
  bookAuthor=NULL;
  if (response_id == GTK_RESPONSE_ACCEPT)
    {

    GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);
    filePath = gtk_file_chooser_get_filename (chooser);
    GFile * selectFile=gtk_file_chooser_get_file (chooser);
      fileName=g_file_get_basename(selectFile);


      fileExt=strrchr(fileName,'.');
      book_log_debug("fileExt=%s,cmp=%d\n",fileExt,strcmp(fileExt,".pdf"));
      if(strcmp(fileExt,".pdf")==0)
        {

        BookPdf *bookPdf=book_pdf_new(filePath,NULL,&error);
           if(error!=NULL)
      {
          if(error->code ==1 && strcmp("Document is encrypted",error->message)==0)
          {
            book_log_error ("文件需要密码才能打开:%s\n",error->message);
             inputPwd=book_dialogs_return_password(NULL,NULL);
            if(inputPwd!=NULL)
              {
                g_clear_object(&bookPdf);
                 bookPdf=book_pdf_new(filePath,inputPwd,&error);
                g_free(inputPwd);
              }
          }
      }

        bookTitle= book_pdf_get_metadata (bookPdf, BOOK_PDF_META_TITLE);
          bookAuthor= book_pdf_get_metadata (bookPdf, BOOK_PDF_META_AUTHOR);
           md5= g_compute_checksum_for_string(G_CHECKSUM_MD5,filePath,-1);
          if(bookTitle==NULL || strlen(bookTitle)<1)
            {
              bookTitle=g_strdup(fileName);
            }
          book_manage_insert_bookmeta (md5, bookTitle, bookAuthor, filePath,"-4",NULL);
            g_printf("打开的文件：filePath=%s,fileName=%s,md5=%s,bookTitle=%s,author=%s \n",filePath,fileName,md5,bookTitle,bookAuthor);
        }

 //book_manage_insert_bookmeta (bookId, bookTitle, bookAuthor, filePath,NULL,NULL);
    g_free (fileName);
      g_free(bookTitle);
      g_error_free(error);
    }

  gtk_native_dialog_destroy (GTK_NATIVE_DIALOG (native));
  g_object_unref (native);
}

void book_application_import_dir_open (GSimpleAction *action, GVariant *parameter,gpointer user_data){

   GApplication *app = user_data;
  GtkFileChooserNative *native;

  native = gtk_file_chooser_native_new ("选择目录",
                                        NULL,
                                        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                        "_Open",
                                        "_Cancel");

  g_object_set_data_full (G_OBJECT (native), "app", g_object_ref (app), g_object_unref);
  g_signal_connect (native,
                    "response",
                    G_CALLBACK (book_application_import_open_folder_response_cb),
                    native);


  gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));

}

void book_application_import_file_open (GtkWidget *widget, GdkEvent *event){

 GtkFileChooserNative *native;

  native = gtk_file_chooser_native_new ("选择文件",
                                        NULL,
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        "_Open",
                                        "_Cancel");

  g_signal_connect (native,
                    "response",
                    G_CALLBACK (book_application_import_open_response_cb),
                    native);

//后缀为.epub的文件
    GtkFileFilter* filter = gtk_file_filter_new();
gtk_file_filter_set_name (filter, ("All files"));
    gtk_file_filter_add_pattern(filter,"*.[Ee][Pp][Uu][Bb]");
 gtk_file_filter_add_pattern(filter,"*.[Mm][Oo][Bb][Ii]");
gtk_file_filter_add_pattern(filter,"*.[Aa][Zz][Ww][3]");
  gtk_file_filter_add_pattern(filter,"*.[Pp][Dd][Ff]");
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (native),filter);

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name (filter, ("epub"));
    gtk_file_filter_add_pattern(filter,"*.[Ee][Pp][Uu][Bb]");
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (native),filter);

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name (filter,("mobi"));
    gtk_file_filter_add_pattern(filter,"*.[Mm][Oo][Bb][Ii]");
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (native),filter);

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name (filter,("azw3"));
    gtk_file_filter_add_pattern(filter,"*.[Aa][Zz][Ww][3]");
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (native),filter);

   filter = gtk_file_filter_new();
    gtk_file_filter_set_name (filter,("pdf"));
    gtk_file_filter_add_pattern(filter,"*.[Pp][Dd][Ff]");
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (native),filter);




  gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));

}


 

#include <libnotify/notify.h>
#include <stdio.h>
#include <unistd.h>

int
main ()
{
        NotifyNotification *n;

        notify_init ("Markup");

        n = notify_notification_new ("通知模拟",
                                     "Some <b>bold</b>, <u>underlined</u>, <i>italic</i>, "
                                     "<a href='http://www.google.com'>linked</a> text",
                                     NULL);
        notify_notification_set_timeout (n, 3000);      //3 seconds

        if (!notify_notification_show (n, NULL)) {
                fprintf (stderr, "failed to send notification\n");
                return 1;
        }

        return 0;
}


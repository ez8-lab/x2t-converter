//#include <QCoreApplication>

#include "../../src/ui/base/clipboard/clipboard.h"
#include <iostream>
#include "gtk/gtk.h"

// Qt debug (UBUNTU):
// sudo sysctl kernel.yama.ptrace_scope=0
// sudo apt-get install libx11-dev
// sudo apt-get install libglib2.0-dev
// sudo apt-get install libgtk2.0-0 libgtk2.0-dev


int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);



    gtk_init (&argc, &argv);

    ui::Clipboard *pClipboard = ui::Clipboard::GetForCurrentThread();

        bool bIsHtmlAvailable = pClipboard->IsFormatAvailable(ui::Clipboard::GetHtmlFormatType(), ui::Clipboard::BUFFER_STANDARD);
        bool bIsTextAvailable = pClipboard->IsFormatAvailable(ui::Clipboard::GetPlainTextFormatType(), ui::Clipboard::BUFFER_STANDARD);
        if (bIsHtmlAvailable)
        {
            base::string16 markup;
            std::string src_url;
            uint32 fragment_start;
            uint32 fragment_end;

            //pClipboard->ReadHTML(ui::Clipboard::BUFFER_STANDARD, &markup, &src_url, &fragment_start, &fragment_end);
            pClipboard->ReadDoct (ui::Clipboard::BUFFER_STANDARD, &markup);
            std::cout << "SRC Url: " << src_url << std::endl;
            std::cout << "Markup: " << markup << std::endl;

        }
        ui::Clipboard::DestroyClipboardForCurrentThread (pClipboard);

        //return a.exec();
        return 0;
}
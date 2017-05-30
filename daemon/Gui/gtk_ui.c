/**
 * @file
 * The main function of the GUI program
 * for the daemon of system's sound control
 *
 **
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Daniel Haimov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Log.h"
#include "init.h"
#include "ConnectionTypes.h"
#include "file_utils.h"

#define WIN_TITLE "Soundroid"  /**< The title of the program's window */
#define ERR -1                 /**< The code of an error */

/**
 * Get from file the last connection type (WIFI/BLUETOOTH)
 * used by daemon
 * @return The last connection type
 */
const ConnType getLastConnectionType()
{
    int connTypeNum = WIFI;

    if(access(FILE_NAME_OF_LAST_USED_CONNECTION_TYPE, F_OK) != ERR)
	{
	    const size_t LEN = 2;
	    char *str = (char*) calloc(LEN, sizeof(char));
	    if(cpStrFromFileToBuff(FILE_NAME_OF_LAST_USED_CONNECTION_TYPE, LEN, str))
		connTypeNum = atoi(str);
	    
	    free(str);
	} 
    
    return (ConnType)connTypeNum;
}

/**
 * The main function
 */
int main (int argc, char *argv[])
{
    curConnType = getLastConnectionType();
    
    gtk_init (&argc, &argv);

    openLogFile("log_gui.txt");

    GtkWidget *vBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    GtkWidget *frameLocal = initFrameLocal();
    gtk_container_add (GTK_CONTAINER (vBox), frameLocal);

    GtkWidget *frameConnected = initFrameConnected();
    gtk_container_add (GTK_CONTAINER (vBox), frameConnected);

    GtkWidget *frameDaemonState = initDaemonStateFrame();
    gtk_container_add (GTK_CONTAINER (vBox), frameDaemonState);

    GtkWidget *frameButtons = initButtonsFrame();
    gtk_container_add (GTK_CONTAINER (vBox), frameButtons);

    initStatusIcon();
    
    GtkWidget *mainWin = initWindow(WIN_TITLE);

    initClosingWinButt(frameButtons, "Close", (gpointer)mainWin);
    
    gtk_container_add (GTK_CONTAINER (mainWin), vBox);
    gtk_widget_show_all (mainWin);

    hideWidgetsByConnectionType();
    
    gtk_main ();
   
    closeLogFile();
    return 0;
}

/**
 * @file
 * Initialization of GUI: main window, its frames and their elements
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

#include "init.h"
#include "port_ops.h"
#include "file_utils.h"
#include "str_utils.h"

#include <string.h>
#include <stdlib.h>

#define ICON_PATH "Sound.png"                                                 /**< The path to the program's icon */
 
#define STOP_DAEMON_SCRIPT_NAME "stop.sh"                                     /**< The name of the script for stopping the daemon */
#define DAEMON_PROG_NAME      "vol_daemon"                                    /**< The of the daemon file */

#define PID_FILE_NAME "pid.txt"                                               /**< The name of the file for saving the process ID of the running daemon */

#define PID_NUM_MAXLEN 20                                                     /**< The maximal length of the running daemon's PID */

#define RED_BOLD_MARKUP  "<span weight=\"bold\" color='red'>%s</span>"        /**< The gtk+ text tag for marking up a text to red color and to bold font */
#define BLUE_BOLD_MARKUP "<span weight=\"bold\" color='blue'>%s</span>"       /**< The gtk+ text tag for marking up a text to blue color and to bold font */

#define PORT_NUM_MAX_LEN 10                                                   /**< The maxima length of the port number */
char prevPortNumStr[PORT_NUM_MAX_LEN] = {'\0'};                               /**< The array of the previous port number string */

GtkWidget *portEditTxt    = NULL;                                             /**< The edit text widget of the port number */

GtkWidget *mainWin        = NULL;                                             /**< The main window of the program. It initialized from the main function */

GtkWidget *daemonStateLbl = NULL;                                             /**< The text label of daemon's state: Running or Stopped */

GtkWidget *localIpLbl     = NULL;                                             /**< The text label of the local IP */
GtkWidget *connectedIpLbl = NULL;                                             /**< The text label of the connected client IP */

ConnType curConnType = WIFI;                                                  /**< Current connection type: Wifi or Bluetooth */




typedef enum visibility_state { HIDE, SHOW } VisibilityState;                 /**< States of visibility for some UI components */
GtkWidget* disapperingWidgetsArr[] = { NULL, NULL };                          /**< The widgets able to be visible/invisible */


void setDaemonStateLbl(GtkLabel *lbl);

/**
 * Get the PID of a running daemon.
 * The PID is in the file within /proc
 * @return string of the PID or an empty string if there is no running daemon
 */
char* getRunningDaemonPID()
{
    if(doesDirHasFile(".", PID_FILE_NAME))
	{
	    char *pid = (char*) calloc(sizeof(char), PID_NUM_MAXLEN);
	    if(cpStrFromFileToBuff(PID_FILE_NAME, PID_NUM_MAXLEN, pid))
		{
		    if(doesDirHasFile("/proc", pid))
			return pid;
		}
	    free(pid);
	    delFileByPath(PID_FILE_NAME);		
	}
    return "";
}

/**
 * Show dialog with the given title, label text and the message type(error, warning, etc.)
 * @param lblTxt The text of the label
 * @param msgType The type of the message of the dialog
 * @param title The title of the dialog window
 */
void showDialog(const gchar *lblTxt, const GtkMessageType msgType, const gchar *title)
{
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(mainWin),
					       GTK_DIALOG_DESTROY_WITH_PARENT,
					       msgType,
					       GTK_BUTTONS_CLOSE,
					       "%s", lblTxt);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(mainWin));
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

/**
 * Stop the daemon
 * @param window (not used)
 * @param data (not used)
 */
static void stopDaemon(GtkWidget *window, gpointer data) 
{
    runFileInCurDir(1, (const char *[]) {STOP_DAEMON_SCRIPT_NAME});
    char *pid = getRunningDaemonPID();
    if(strlen(pid) != 0)
	{
	    showDialog("The volume control daemon haven't stopped!", GTK_MESSAGE_ERROR, "Oops!");
	    free(pid);
	}
    else
	{
	    showDialog("The volume control daemon have stopped!", GTK_MESSAGE_INFO, "Success");
	    setDaemonStateLbl(GTK_LABEL(daemonStateLbl));
	}
}

/**
 * Show the dialog window of stopping daemon
 * @return The code of the clicked button at dialog
 */
const gint showStopDaemonDialog()
{
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(mainWin),
					       GTK_DIALOG_DESTROY_WITH_PARENT,
					       GTK_MESSAGE_WARNING,
					       GTK_BUTTONS_YES_NO,
					       "%s", "Another instance of a remote volume control daemon is stil running.\nStop it?");
    gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
    const gint clickedButt = gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);

    return clickedButt;
}

/**
 * Stop the previous daemon before the new one start
 * @return true The previous daemon has stopped successfully
 */
const bool stopPrevDaemonIfRuns()
{
    bool result = true;
    char* pid = getRunningDaemonPID();
    if(strlen(pid) != 0)
    	{
	    const bool stopDaemon = (showStopDaemonDialog() == GTK_RESPONSE_YES);
	    if(stopDaemon)
		{
		    runFileInCurDir(1, (const char* []) {STOP_DAEMON_SCRIPT_NAME});
		    sleep(1);
		    result = !doesDirHasFile("/proc", pid);
		}
	    free(pid);
	}
    return result;
}

/**
 * Is daemon running?
 * @return true The daemon is running
 */
const bool isDaemonRunning()
{
    char* pid = getRunningDaemonPID();
    const bool result = (strlen(pid) != 0);

    if(result)
	free(pid);

    return result;
}

/**
 * Get the text string of IP for a text label
 * @param ipStr The string of the IP
 * @param noIpStr The string for the not found IP
 * @param lblName The name of the text label
 * @return The text string of IP
 */
gchar* getIpStrForLbl(char *ipStr, char* noIpStr, char* lblName)
{
    if( (ipStr == NULL) || (strlen(ipStr) == 0) )
	ipStr = noIpStr;
    
    size_t label_len = strlen(ipStr) + strlen(lblName) + 1;
    gchar *label = (gchar*)calloc(sizeof(gchar), label_len);
    strcpy(label, lblName);
    strcat(label, ipStr);

    return label;
}

/**
 * Set the text label for the local IP
 * @return The text label widget
 */
GtkWidget* setLocalIpLbl()
{
    char* localIpStr = getIpStrForLbl(getLocalIP(), "NOT_FOUND", "Local IP: ");

    if(localIpLbl == NULL)
	localIpLbl = gtk_label_new(localIpStr);
    else
	gtk_label_set_text(GTK_LABEL(localIpLbl), localIpStr);
    
    free(localIpStr);
    return localIpLbl;
}

/**
 * Start the blue tooth daemon
 * The dialog with a result of starting will be shown
 */
void startBtDaemon()
{
    if(runFileInCurDir(2, (const char* []){DAEMON_PROG_NAME, "bt"}))
	{
	    sleep(1);                    /// TODO should be fixed!!!
	    if(isDaemonRunning())
		{
		    showDialog("The volume control daemon has started", GTK_MESSAGE_INFO, "Started");
		    setDaemonStateLbl(GTK_LABEL(daemonStateLbl));
		    localIpLbl = setLocalIpLbl();
		}
	    else
		showDialog("The volume control daemon has NOT started.\nSee log file", GTK_MESSAGE_ERROR, "ERROR");
	}
    else
	showDialog("The volume control daemon has NOT started.\nSee log file", GTK_MESSAGE_ERROR, "ERROR");
}

/**
 * Start the wifi daemon
 * The dialog with a result of starting will be shown
 */
void startWifiDaemon()
{
    if(portEditTxt != NULL)
	{
	    const gchar *portNumStr = gtk_entry_get_text(GTK_ENTRY(portEditTxt));
	    if(strlen(portNumStr) == 0)
		showDialog("The given port number is EMPTY!", GTK_MESSAGE_ERROR, "ERROR");
	    else if(runFileInCurDir(3, (const char* []){DAEMON_PROG_NAME, "wifi", portNumStr}))
		{
		    sleep(1);                    /// TODO should be fixed!!!
		    if(isDaemonRunning())
			{
			    showDialog("The volume control daemon has started", GTK_MESSAGE_INFO, "Started");
			    setDaemonStateLbl(GTK_LABEL(daemonStateLbl));
			    localIpLbl = setLocalIpLbl();
			}
		    else
			showDialog("The volume control daemon has NOT started.\nSee log file", GTK_MESSAGE_ERROR, "ERROR");
		}
	    else
		showDialog("The volume control daemon has NOT started.\nSee log file", GTK_MESSAGE_ERROR, "ERROR");
	}
    else
	fprintf(stderr, "The port edit text widget is NULL\n");
}

/**
 * Start daemon
 * @param window (not used)
 * @param data (not used)
 */
void startDaemon(GtkWidget *window, gpointer data) 
{
    const bool prevDaemonStopped = stopPrevDaemonIfRuns();
    if(prevDaemonStopped)
	{
	    if(curConnType == WIFI)
		startWifiDaemon();
	    else if (curConnType == BLUETOOTH)
		startBtDaemon();
	    else
		fprintf(stderr, "ERROR: unknown connection type: %d", curConnType);
	}
    else
	showDialog("There is another volume control daemon running.\nIt should be stopped", GTK_MESSAGE_INFO, "Oops!");    	    
}

/**
 * Initialize a button with the given text, for the given container and with the given pointer to function
 * @param container The widget for containing the button
 * @param txt The text of the button
 * @param func The button's function
 * @param data The additional data's pointer
 */
GtkWidget* initButton(GtkWidget *container, char *txt, void (*func) (GtkWidget *window, gpointer data), gpointer data)
{
    GtkWidget* button = gtk_button_new_with_label(txt);
    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (*func),data);
    gtk_container_add(GTK_CONTAINER(container), button);
    return button;
}


/**
 * Initialize a radio button with the given text, for the given container, with the given pointer to function
 * and the given first radio button
 * @param firstRadio If itsn't NULL the the new radion button will be connected to the existing radio button's group
 * @param container The widget for containing the button
 * @param txt The text of the button
 * @param func The button's function
 * @param data The additional data's pointer
 */ 
GtkWidget* initRadioButt(GtkWidget *container, char *txt, GtkWidget *firstRadio, void (*func) (GtkWidget *window, gpointer data), gpointer data)
{
    GtkWidget *radio = NULL;
    radio = (firstRadio == NULL) ? gtk_radio_button_new_with_label (NULL, txt): 
	                           gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(firstRadio), txt);

    g_signal_connect (G_OBJECT (radio), "clicked", G_CALLBACK (*func),data);
    gtk_box_pack_start (GTK_BOX (container), radio, TRUE, TRUE, 2);
    
    return radio;
}

/**
 * Initialize the edit text widget with option to be disappering
 * @param container The container for containing the edit text widget
 * @param labelTxt The text for the label of the edit text widget
 * @param txt The text for the edit text widget
 * @param disapperingWidgetsArrInd The index in the array of pointers to disapperaing widgets
 * @return The initialized edit text widget
 */
GtkWidget* initEditTxt(GtkWidget *container, char *labelTxt, const gchar *txt, const int disapperingWidgetsArrInd)
{
    GtkWidget *hBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *lbl  = gtk_label_new(labelTxt);
    
    GtkWidget *entry = gtk_entry_new();
    if( (txt != NULL) && (strlen(txt) != 0) )
	gtk_entry_set_text(GTK_ENTRY(entry), txt);
    
    gtk_box_pack_end (GTK_BOX (hBox), entry, FALSE, FALSE, 2);
    gtk_box_pack_end (GTK_BOX (hBox), lbl, FALSE, FALSE, 2);
    gtk_box_pack_end (GTK_BOX (container), hBox, FALSE, FALSE, 5);

    disapperingWidgetsArr[disapperingWidgetsArrInd] = hBox;
    return entry;
}

/**
 * Change port function
 * @param window (not used)
 * @param data (not used)
 */
void chgPort(GtkWidget *window, gpointer data) 
{
    const gchar *port = gtk_entry_get_text(GTK_ENTRY(portEditTxt));
    if(strlen(port) == 0)
	{
	    showDialog("The new port number is EMPTY", GTK_MESSAGE_WARNING, "Empty port");
	    return;
	}
    
    if(strcmp(port, prevPortNumStr) == 0)
	{
	    showDialog("You have not changed the port number", GTK_MESSAGE_INFO, "Same port");
	    return;
	}
    
    char *pid = getRunningDaemonPID();
    if(strlen(pid) != 0)
	{
	    if(!chgPortOfDaemon(port))
		{
		    showDialog("Can't change the port number!'.\nSee log file", GTK_MESSAGE_ERROR, "ERROR");
		    gtk_entry_set_text(GTK_ENTRY(portEditTxt), prevPortNumStr);
		}
	    else
		{
		    showDialog("Port has been changed", GTK_MESSAGE_INFO, "Port changed");
		    bzero(prevPortNumStr, PORT_NUM_MAX_LEN);
		    strcpy(prevPortNumStr, port);
		}
	    free(pid);
	}
    else    //    else if(!runFileInCurDir(DAEMON_SCRIPT_NAME, port))
	showDialog("The volume control daemon has NOT started", GTK_MESSAGE_WARNING, "Daemon isn't running");
}

/**
 * Set the text of the given text label and its color
 * @param lbl The text label widget
 * @param markup_str The string for marking up the text
 * @param txt The text for the label
 */
void setLblTxtAndColor(GtkLabel *lbl, const char *markup_str, const char *txt)
{
    if(lbl != NULL)
	{
	    if(!isStrEmptyOrNULL(markup_str, __FILE__, __LINE__))
		if(!isStrEmptyOrNULL(txt, __FILE__, __LINE__))
		    {
			char *markup = g_markup_printf_escaped(markup_str, txt);
			gtk_label_set_markup(lbl, markup);
			g_free(markup);
		    }		    
	}
    else
	fprintf(stderr, "ERROR: The given label pointer is NULL in %s: %d\n", __FILE__, __LINE__);
}

/**
 * Set the label of the daemon's state
 * @param lbl The label widget
 */
void setDaemonStateLbl(GtkLabel *lbl)
{
    if(lbl != NULL)
	{
	    char *pid = getRunningDaemonPID();
	    
	    const bool daemonRun = (strlen(pid) != 0);
	    const char *markup = daemonRun ? BLUE_BOLD_MARKUP: RED_BOLD_MARKUP;
	    const char *txt    = daemonRun ? "Running": "Stopped";
	    setLblTxtAndColor(lbl, markup, txt);
	    
	    if(daemonRun)
		free(pid);
	}
    else
	fprintf(stderr, "ERROR: The given label pointer is NULL in %s: %d\n", __FILE__, __LINE__);
}

/**
 * Initialize the frame of daemon's state
 * @return The frame's widget
 */
GtkWidget* initDaemonStateFrame()
{
    GtkWidget *frame = gtk_frame_new("Volume control daemon state");
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    GtkWidget *hBox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    daemonStateLbl = gtk_label_new(NULL);
    setDaemonStateLbl(GTK_LABEL(daemonStateLbl));
    gtk_box_pack_end (GTK_BOX (hBox1), daemonStateLbl, TRUE, TRUE, 5);
    
    gtk_container_add (GTK_CONTAINER (frame), hBox1);
    
    return frame;
}

/**
 * Change the visibility state of the widget with visibility property
 * @param state The state of visibility: HIDE or SHOW
 */
void chgDisappearedWidgetsState(const VisibilityState state) 
{
    size_t size = sizeof(disapperingWidgetsArr) / sizeof(disapperingWidgetsArr[0]);
    int i;
    for(i = 0; i < size; ++i)
	{
	    if(disapperingWidgetsArr[i] == NULL)
		{
		    fprintf(stderr, "ERROR: current pointer number %d to a disappearing widget is NULL\n", i);
		    return;
		}
	    switch(state)
		{
		case HIDE:
		    gtk_widget_hide(disapperingWidgetsArr[i]);
		    break;
		case SHOW:
		    gtk_widget_show(disapperingWidgetsArr[i]);
		    break;
		default:
		    fprintf(stderr, "ERROR: Unknown state of visibility\n");
		    return;
		}
	}
}

/**
 * Set connection type WiFi 
 * @param butt The button of the connection type
 * @param data (not used)
 */
void setConnectionTypeWifi(GtkWidget *butt, gpointer data)
{
    curConnType = WIFI;
    chgDisappearedWidgetsState(SHOW);
}

/**
 * Set connection type Blue Tooth
 * @param butt The button of the connection type
 * @param data The adding data's pointer
 */
void setConnectionTypeBlueTooth(GtkWidget *butt, gpointer data)
{
    curConnType = BLUETOOTH;
    chgDisappearedWidgetsState(HIDE);
}

/**
 * Initialize the radio button of connection type the current connection type
 * @param container The container with the radio buttons
 */
void initRadioButtsByConnType(GtkWidget *container)
{
    if(curConnType == WIFI)
	{
	    GtkWidget *radioWiFi = initRadioButt(container, "WiFi", NULL, &setConnectionTypeWifi, NULL);
	    initRadioButt(container, "BlueTooth", radioWiFi, &setConnectionTypeBlueTooth, radioWiFi);
	}
    else if(curConnType == BLUETOOTH)
	{
	    GtkWidget *radioBT = initRadioButt(container, "BlueTooth", NULL, &setConnectionTypeBlueTooth, NULL);
	    initRadioButt(container, "WiFi", radioBT, &setConnectionTypeWifi, radioBT);		
	}
    else
	{
	    fprintf(stderr, "ERROR: Unknown connection type: %d\n", curConnType);
	}
}

/**
 * Initialize the frame with the info of local parameters: local IP, used connection type
 * @return The built frame widget
 */
GtkWidget* initFrameLocal()
{
    GtkWidget *frame = gtk_frame_new("Local");
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    GtkWidget *hBox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    
    localIpLbl = setLocalIpLbl();
    gtk_box_pack_end (GTK_BOX (hBox1), localIpLbl, FALSE, FALSE, 5);

    initRadioButtsByConnType(hBox1);
    gtk_container_add (GTK_CONTAINER (frame), hBox1);
    
    return frame;
}

/**
 * Set the label for connected IP
 * @return The label of the connected IP
 */
GtkWidget* setConnectedIpLbl()
{
    char* connectedIpStr = getIpStrForLbl(getConnectedIP(), "None", "Connected IP: ");

    if(connectedIpLbl == NULL)
	connectedIpLbl = gtk_label_new(connectedIpStr);
    else
	gtk_label_set_text(GTK_LABEL(connectedIpLbl), connectedIpStr);
    
    free(connectedIpStr);
    return connectedIpLbl;
}

/**
 * Initialisation of the frame 'Connected'
 * @return The initialised frame
 */
GtkWidget* initFrameConnected()
{
    GtkWidget *hBox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    connectedIpLbl = setConnectedIpLbl();
    gtk_container_add (GTK_CONTAINER (hBox2), connectedIpLbl);

    strcpy(prevPortNumStr, getPortOfDaemon());
    portEditTxt = initEditTxt(hBox2, "Port: ", prevPortNumStr, 0);
    
    GtkWidget *frame = gtk_frame_new("Connected");
    gtk_container_add (GTK_CONTAINER (frame), hBox2);

    return frame;
}

/**
 * Init the status icon of the program
 */
void initStatusIcon()
{
    GtkStatusIcon *status_icon = gtk_status_icon_new_from_file(ICON_PATH);
    gtk_status_icon_set_visible(status_icon, TRUE);
    gtk_status_icon_set_tooltip_text(GTK_STATUS_ICON(status_icon), "PC sound control from Android");
}

/**
 * Close the window with the given pointer
 * @param widget (not used)
 * @param window The being closed window pointer
 */
void close_window(GtkWidget *widget, gpointer window)
{
    gtk_widget_destroy(GTK_WIDGET(window));
    gtk_main_quit();
}

/**
 * Iinitialise the button for closing the given window. Add
 * the button to the given frame
 * @param frame The frame for adding the button to
 * @param lblTxt The text of the button's label
 * @param window The window should be closed by the buttons
 */
void initClosingWinButt(GtkWidget *frame, gchar *lblTxt, gpointer window)
{
    GtkWidget* button = gtk_button_new_with_label(lblTxt);
    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
    g_signal_connect(G_OBJECT (button), "clicked", G_CALLBACK (*close_window), window);
    //    gtk_container_add(GTK_CONTAINER(frame), button);
    gtk_box_pack_end (GTK_BOX (frame), button, FALSE, FALSE, 5);
}

/**
 * Iinitialise the frame of buttons
 * @return The initialised frame with buttons
 */
GtkWidget* initButtonsFrame()
{
    GtkWidget *frame = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    initButton(frame, "Stop", &stopDaemon, NULL);
    initButton(frame, "Start", &startDaemon, NULL);
    GtkWidget* butt = initButton(frame, "Change Port", &chgPort, NULL);
    disapperingWidgetsArr[1] = butt;
    return frame;
}

/**
 * Close this application function.  The function of gtk+
 * @param widget (not used)
 * @param event (not used)
 * @param data (not used)
 */
gint close_application( GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    gtk_main_quit();
    return(FALSE);
}

/**
 * Initialise a window with the given title
 * @param title The title string
 * @return The initialised window widget
 */
GtkWidget* initWindow(gchar* title)
{
    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_container_set_border_width (GTK_CONTAINER (window), 25);
    gtk_widget_set_size_request (window, 200, 200);
    g_signal_connect (window, "delete-event", G_CALLBACK (close_application), NULL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    return window;
}

/**
 * Hide widgets by the current connection type
 */
void hideWidgetsByConnectionType()
{
    switch(curConnType)
	{
	  case BLUETOOTH:
	      chgDisappearedWidgetsState(HIDE);
	      break;
	  case WIFI:
	      chgDisappearedWidgetsState(SHOW);
	      break;
	  default:
	      printf("Unknown connection type %d\n", curConnType);
	}    
}

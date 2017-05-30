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


#ifndef INIT_HEADER
#define INIT_HEADER

#include <gtk/gtk.h>

/**
 * \enum connection_type
 * \brief Connection types
 */
typedef enum connection_type { WIFI, BLUETOOTH } ConnType;


extern GtkWidget *mainWin;                    /**< The main window of the program. It initialized from the main function */
extern ConnType curConnType;                  /**< The currently used connection type */

/**
 * Initialize the frame with the info of local parameters: local IP, used connection type
 * @return The built frame widget
 */
GtkWidget* initFrameLocal();

/**
 * Initialisation of the frame 'Connected'
 * @return The initialised frame
 */
GtkWidget* initFrameConnected();

/**
 * Iinitialise the frame of buttons
 * @return The initialised frame with buttons
 */
GtkWidget* initButtonsFrame();

/**
 * Initialize the frame of daemon's state
 * @return The frame's widget
 */
GtkWidget* initDaemonStateFrame();

/**
 * Iinitialise the button for closing the given window. Add
 * the button to the given frame
 * @param frame The frame for adding the button to
 * @param lblTxt The text of the button's label
 * @param window The window should be closed by the buttons
 */
void initClosingWinButt(GtkWidget *frame, gchar *lblTxt, gpointer window);

/**
 * Initialise a window with the given title
 * @param title The title string
 * @return The initialised window widget
 */
GtkWidget* initWindow(gchar* title);

/**
 * Init the status icon of the program
 */
void initStatusIcon();

/**
 * Hide or show widgets dependengly on the current connection type
 */
void hideWidgetsByConnectionType();

#endif

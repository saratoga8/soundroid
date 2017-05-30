/**
 * @file
 * The main activity of the application
 * 
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

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package com.soundroid.ui;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

import com.soundroid.R;
import com.soundroid.net.NetConnector;

import java.io.IOException;

/**
 * The activity for selecting desktop and changing volume for the selected one 
 */
public class DistanceVolumeActivity extends Activity {

    private String TAG = "DistanceVolumeActivity";                                         /**< The tag for writing to log file */
	
    private NetConnector connector;                                                        /**< The connector to a network */

    private UI ui;                                                                         /**< The UI instance */

    private ConnectionTypeObserver connectionTypeObserver;                                 /**< The observer of connection type changing */

    private static final int CONNECTION_TYPE_CHANGED = 1;
    private static final int REDRAW_UI               = 2;
    private static final int REDRAW_SOUNDVOL_UI      = 3;

    private Thread observerThread = null;                                                   /**< The thread of UI observing */


	/**
	 * Save the currently selected desktop name
	 */
    @Override
	public void onPause() {
		super.onPause();
		Log.d(TAG, "PAUSE");

//        ui.updateCurSelectedDesktopName();
//		ui.redraw(true);
	}

	/**
	 * Close connection to a desktop
	 */
    @Override
	public void	onStop() {
		super.onStop();
		Log.d(TAG, "STOP");

        connectionTypeObserver.stopObserving();
        observerThread = null;

		if(connector != null)
			connector.closeConnection();
	}	

	/**
	 * If should be redrawn, check WiFi connection.
	 * If there is WiFi, initialize UI
	 */
    @Override
	public void onStart() {
		super.onStart();
		Log.d(TAG, "START");
        reDrawUI();
        if(observerThread == null) {
            connectionTypeObserver = new ConnectionTypeObserver();
            observerThread = new Thread(connectionTypeObserver);
            observerThread.start();
        }
    }

    /**
     * Redraw the UI
     */
    private void reDrawUI() {
        setContentView(R.layout.main);

        boolean connected = connector.isConnectedToNet(this);
        if(!connected) {
            connected = isConnectedToNetAfterDialog();
            if(connected)
                ui.initDesktopsDatasFromFile();
        }

        ui.initUI(connected);
        ui.redraw(false);
    }

    /**
     * Is there a connection to a network after opening the dialog for user
     * @return true Connected to a network
     */
    private boolean isConnectedToNetAfterDialog() {
        final int TRYING_NUM = 10, SLEEP_MS = 300;
        ui.connectToNetByDialog();
        for (int i = 0; i < TRYING_NUM; ++i) {
            if(!connector.isConnectedToNet(this)) {
                try {
                    Thread.sleep(SLEEP_MS);
                } catch (InterruptedException e) {
                    Log.e(TAG, "Sleep exception: " + e.toString());
                    break;
                }
            }
            else
                return true;
        }
        Toast.makeText(this, "Hasn't connected after " + TRYING_NUM * SLEEP_MS / 1000 + " seconds", Toast.LENGTH_LONG).show();
        return false;
    }

    /**
     * Initialize the desktops datas from file, previously selected connection type.
     * Initialize connection
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        Log.d(TAG, "CREATE");

        initUiFromFile();
    }

    /**
     * Initialize UI from file
     */
    private void initUiFromFile() {
        try {
            ui = UI.buildUiFromFile(this);
            connector = ui.getConnector();
        } catch (IOException e) {
            Log.e(TAG, "Can't build UI");
            finish();
        }
        ui.initDesktopsDatasFromFile();
        ui.redraw(true);
    }

    /**
     * Initialize UI with dependence on the connection type radio button selected
     */
    private void initUiByRadioButt() {
        ui = ui.buildUiBySelectedRadioButt(this);
        connector = ui.getConnector();
        ui.initDesktopsDatasFromFile();
        reDrawUI();
    }

	/**
	 * If there is connection to a desktop, close the connection
	 * and finish the activity
	 */
    @Override
	public void onBackPressed() {
        closeConnector();
		finish();
	}

	/**
	 * Save desktops data and close connection
	 */
    @Override
	public void onDestroy() {	
		Log.d(TAG, "DESTROY");
		
		ui.saveUiData();
        ui.clrDesktopsData();

        closeConnector();

		super.onDestroy();
	}

    /**
     * Close the connector to a network
     */
    private void closeConnector() {
        if(connector != null) {
//            if(connector.isConnectedToDaemon()) {
//                connector.sendEnd();
//            }
            connector.closeConnection();
        }
    }

	/**
	 * If should be redrawn, initialize the desktops names drop box
	 * and save the the desktops datas
	 */
    @Override
	protected void onResume() {
	   super.onResume();
	   Log.d(TAG, "RESUME"); /*
	   if(ui.shouldBeRedrawn()) {
		   onStart();
		   ui.initDesktopNamesSpinner();
		   ui.saveDesktopsDatas();
		   ui.redraw(false);
	   }*/
	}

	/**
	 * Processing the results from the over activities 
	 */
    @Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(resultCode == RESULT_OK) {
            ui.processActivityResult(requestCode, data);
        }
	}

    /**
     * The handler of UI events messages
     */
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case CONNECTION_TYPE_CHANGED:
                    closeConnector();
                    initUiByRadioButt();
                    break;
                case REDRAW_UI:
                    reDrawUI();
                    break;
                case REDRAW_SOUNDVOL_UI:
                    ui.setVolumeUI();
                    break;
                default:
                    Log.e(TAG, "Unknown message: " + msg.what);
            }
        }
    };

    /**
     * The observing of connection to a network changes
     */
    private class ConnectionTypeObserver implements Runnable {
        private boolean stop = true;                                /**< Stop observing flag */

        /**
         * Stop observing
         */
        public synchronized void stopObserving() {
            stop = true;
        }

        /**
         * Is observing stopped
         * @return true Stopped
         */
        public synchronized boolean isStopped() {
            return stop;
        }

        @Override
        public void run() {
            stop = false;
            while(!isStopped()) {
                if(ui.hasConnectionTypeChanged()) {
                    sleepBeforeChangingConnectorType(500);
                    ui.setConnectionTypeChanged(false);
                    handler.sendEmptyMessage(CONNECTION_TYPE_CHANGED);
                }
                else if(ui.shouldBeRedrawn()) {
                    ui.redraw(false);
                    handler.sendEmptyMessage(REDRAW_UI);
                }
                else if (ui.shouldBeRedrawnSoundVolUI()) {
                    ui.redrawSoundVolUI(false);
                    handler.sendEmptyMessage(REDRAW_SOUNDVOL_UI);
                }
            }
        }

        /**
         * Sleep the given type before changing connection type
         * @param time Time in milli seconds
         */
        private void sleepBeforeChangingConnectorType(final int time) {
            try {
                Thread.sleep(time);
            } catch (InterruptedException e) {
                Log.e(TAG, "Can't sleep before changing connector type: " + e.toString());
            }
        }
    }
}
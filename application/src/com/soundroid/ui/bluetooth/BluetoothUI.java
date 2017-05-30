/**
 * @file
 * The user interface for remote sound control of a desktop by bluetooth
 *
 * *
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


package com.soundroid.ui.bluetooth;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.Intent;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.RadioButton;

import com.soundroid.R;
import com.soundroid.net.DesktopData;
import com.soundroid.net.NetConnector;
import com.soundroid.net.bluetooth.BluetoothConnector;
import com.soundroid.net.bluetooth.BluetoothDesktopData;
import com.soundroid.others.FileUtils;
import com.soundroid.ui.UI;

import java.io.IOException;
import java.security.InvalidParameterException;


/**
 * UI for connecting by blue tooth
 */
public class BluetoothUI extends UI {
    private static final String NAMES_DATA_FILE_NAME = "bt.data";              /**< The file containing the desktops data of found remote devices */
    private static final int REQUEST_CODE_FIND_REMOTE_DEV = 24;                /**< The request code for founding remote devices */
    private String TAG = "BluetoothUI";                                        /**< The tag for logs */

    private BluetoothUI() {}

    /**
     * Constructor
     * @param activity Main activity instance
     */
    public BluetoothUI(final Activity activity) {
        Log.d(TAG, "Initialisation");

        this.activity = activity;
        context = activity.getApplicationContext();
        connector = new BluetoothConnector();
    }

    /**
     * Save the type of the current connection(Wifi or bluetooth)
     */
    @Override
    protected void saveCurConnectionType() {
        Log.d(TAG, "Save current connection type Bluetooth to file");

        try {
            final int num = NetConnector.ConnectionTypes.BLUE_TOOTH.ordinal();
            FileUtils.saveDataStr(context, CONN_TYPE_CHECKED_RADIO_BUTT_ID_FILE_NAME, Integer.toString(num), Context.MODE_PRIVATE);
        } catch (IOException e) {
            Log.e(TAG, "\tCan't save Bluetooth connection type to the file " + CONN_TYPE_CHECKED_RADIO_BUTT_ID_FILE_NAME + ": " + e.toString());
        }
    }

    /**
     * Fill the list of desktops names by the data from file
     * @param fileName The name of the file
     * @throws IOException
     */
    @Override
    protected void fillDesktopsListFromFile(final String fileName) throws IOException {
        Log.d(TAG, "Finding paired devs");
        final BluetoothConnector btConnector = (BluetoothConnector)connector;
        btConnector.initPairedDevs();
        for(BluetoothDesktopData data: btConnector.getPairedDesktopsDatas()) {
            Log.d(TAG, "Found " + data.getName());
            desktopsDatas.add(data);
        }
    }

    /**
     * Initialize desktops data from file
     */
    @Override
    public void initDesktopsDatasFromFile() {
        initDesktopsDatasFromFile(NAMES_DATA_FILE_NAME);
        try {
            String dataStr = FileUtils.getOneLineFromFile(activity, NAMES_DATA_FILE_NAME);
            if(!TextUtils.isEmpty(dataStr)) {
                BluetoothDesktopData data = new BluetoothDesktopData(dataStr);
                if(desktopsDatas.contains(data)) {
                    desktopsDatas.remove(data);
                    desktopsDatas.add(0, data);
                }
            }
        } catch (IOException e) {
            Log.e(TAG, "Can't read line from the file " + NAMES_DATA_FILE_NAME);
        }
    }

    /**
     * Save desktops data to file
     */
    @Override
    public void saveDesktopsDatas() {}

    /**
     * Initialize the radio button for selecting connection type bluetooth
     */
    @Override
    protected void initConnectionTypeRadioButt() {
        RadioButton radioButton = (RadioButton)activity.findViewById(R.id.blueToothRadio);
        radioButton.setChecked(true);
//        connectionTypeRadio.check(R.id.blueToothRadio);
    }

    /**
     * Open the activity of connecting
     */
    private void openConnectingActivity(final String connectedDevName) {
        if(TextUtils.isEmpty(connectedDevName)) {
            Log.e(TAG, "The given device name is EMPTY or NULL");
            return;
        }
        final Intent intent = new Intent(context, BlueToothConnectingActivity.class);
        intent.putExtra(BlueToothConnectingActivity.CONNECTED_DEV_NAME, connectedDevName);
        activity.startActivityForResult(intent, REQUEST_CODE_FIND_REMOTE_DEV);
    }

    /**
     * Initialization of the button 'New'
     */
    @Override
    protected void initNewButt() {
        buttNew = (Button) activity.findViewById(R.id.newButt);
        buttNew.setVisibility(View.INVISIBLE);
    }

    /**
     * Initialisation of the button 'Edit'
     */
    @Override
    protected void initEditButt() {
        buttEdit = (Button) activity.findViewById(R.id.editButt);
        buttEdit.setVisibility(View.INVISIBLE);
    }

    /**
     * Process the results of the closed activities
     * @param requestCode The request code
     * @param data The data from the closed activity
     */
    @Override
    public void processActivityResult(final int requestCode, final Intent data) {
        if(requestCode == REQUEST_CODE_FIND_REMOTE_DEV) {
            final boolean devFound = data.getBooleanExtra(BlueToothConnectingActivity.IS_REMOTE_DEV_FOUND, false);
            if(!devFound) {
                showOkDialog("The remote device hasn't discovered");
            }
            else {
                saveConnectedDesktopData();
            }

            redraw(true);

        }
    }

    /**
     * Save the desktop data of the currently connected remote bluetooth device
     */
    private void saveConnectedDesktopData() {
        BluetoothConnector btConnector = (BluetoothConnector) connector;
        String name = btConnector.getConnectedDeviceName();
        String addr = btConnector.getDeviceAddrByName(name);
        try {
            BluetoothDesktopData data = new BluetoothDesktopData(name, addr);
            FileUtils.saveDataStr(activity, NAMES_DATA_FILE_NAME, data.toString(), activity.MODE_PRIVATE);
        }
        catch (InvalidParameterException e) {
            Log.e(TAG, "Can't save bluetooth desktop data: " + e.toString());
        }
        catch (IOException e) {
            Log.e(TAG, "Can't save bluetooth desktop data: " + e.toString());
        }
    }

    /**
     * Turning on bluetooth connection by opening dialog 'Bluetooth permission request' for user
     */
    @Override
    public void connectToNetByDialog() {
        enableBlueTooth();
    }

    /**
     * Re-initialize the bluetooth connector
     */
    @Override
    protected void reInitConnector() {
        connector.closeConnection();
        while (connector.isConnectedToDaemon ());

        String curSelectedDesktopName = getCurSelectedDesktopName();
        if(TextUtils.isEmpty(curSelectedDesktopName)) {
            return;
        }
    }

    /**
     * Get the name of the desktop currently selected in UI
     * @return The name of the desktop or empty string
     */
    private String getCurSelectedDesktopName() {
        try {
            final DesktopData curSelectedDesktopData = desktopsDatas.get(curSelectedDesktopNameInd);
            return curSelectedDesktopData.getName ();
        } catch (IndexOutOfBoundsException e) {
            Log.e(TAG, "The selected index in the spinner is " + curSelectedDesktopNameInd + " and the size of the list of desktops data is " + desktopsDatas.size ());
            return "";
        }
    }

    /**
     * Enable bluetooth connection
     */
    private void enableBlueTooth () {
        final BluetoothAdapter btAdapter = ((BluetoothConnector)connector).getAdapter ();
        if(btAdapter != null) {
            if (!btAdapter.isEnabled()) {
                final Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                activity.startActivity(enableBtIntent);
            }
        }
        else {
            Log.e(TAG, "The device doesn't support blue tooth!");
        }
    }

    /**
     * Set volume percent of the currently selected desktop if there is network connection
     * and there is connection to the desktop
     */
    @Override
    public void setVolumeUI() {
        final DesktopData curSelectedDesktopData = desktopsDatas.get(curSelectedDesktopNameInd);
        final String curSelectedDevName = curSelectedDesktopData.getName();
        if(!TextUtils.isEmpty(curSelectedDevName) && !host_connection_checked) {
            host_connection_checked = true;
            BluetoothConnector btConnector = (BluetoothConnector) connector;
            if (!btConnector.getConnectedDeviceName().equals(curSelectedDevName)) {
                openConnectingActivity(curSelectedDesktopData.getName());
                return;
            }
        }

        try {
            if(connector.isConnectedToDaemon()) {
                setVolumeUiElements();
            }
        }
        catch(Exception e) {
            showOkDialog("Can't connect to the remote device '" + curSelectedDesktopData.getName() + "'");
        }
    }
}

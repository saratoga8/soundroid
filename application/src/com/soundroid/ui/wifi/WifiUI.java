/**
 * @file
 * The user interface for remote sound control of a desktop by wifi
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

package com.soundroid.ui.wifi;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.RadioButton;

import com.soundroid.R;
import com.soundroid.net.DesktopData;
import com.soundroid.net.NetConnector;
import com.soundroid.net.wifi.WifiConnector;
import com.soundroid.net.wifi.WifiDesktopData;
import com.soundroid.others.FileUtils;
import com.soundroid.ui.UI;

import java.io.IOException;
import java.util.ArrayList;

/**
 * UI for connecting by wifi
 */
public class WifiUI extends UI {

    private String TAG = "WifiUI";                                                         /**< The tag for logs */

    public static String NAMES_DATA_FILE_NAME = "wifi.data";                               /**< The name of the file for saving desktops datas */

    private WifiUI() {}

    /**
     * Constructor
     * @param activity The main activity
     */
    public WifiUI(final Activity activity) {
        Log.d(TAG, "Initialisation");

        this.activity = activity;
        context = activity.getApplicationContext();
        connector = new WifiConnector();
        host_connection_checked = false;
    }

    /**
     * Save the ID of the checked radio button 'Connection type'
     */
    @Override
    protected void saveCurConnectionType() {
        Log.d(TAG, "Save current connection type WiFi to file");

        try {
            final int num = NetConnector.ConnectionTypes.WIFI.ordinal();
            FileUtils.saveDataStr(context, CONN_TYPE_CHECKED_RADIO_BUTT_ID_FILE_NAME, Integer.toString(num), Context.MODE_PRIVATE);
        } catch (IOException e) {
            Log.e(TAG, "\tCan't save WiFi connection type to the file " + CONN_TYPE_CHECKED_RADIO_BUTT_ID_FILE_NAME + ": " + e.toString());
        }
    }

    /**
     * Fill the list of desktops names by the data from file
     * @param fileName The name of the file
     * @throws IOException
     */
    @Override
    protected void fillDesktopsListFromFile(final String fileName) throws IOException {
        ArrayList<String> datasLines = FileUtils.getLinesFromFile(context, fileName);
        for(String dataStr: datasLines) {
            desktopsDatas.add(new WifiDesktopData(dataStr));
        }
    }

    /**
     * Initialize the radio button for selecting connection type wifi
     */
    @Override
    protected void initConnectionTypeRadioButt() {
        RadioButton radioButton = (RadioButton)activity.findViewById(R.id.wifiRadio);
        radioButton.setChecked(true);
//        connectionTypeRadio.check(R.id.wifiRadio);
    }

    /**
     * Open the activity of connecting to the remote desktop
     * @param desktopData The data of the desktop which connecting to
     */
    private void openConnectingActivity(final DesktopData desktopData) {
        Intent intent = new Intent(context, WiFiConnectingActivity.class);
        intent.putExtra(DESKTOPS_DATA, desktopData);

        activity.startActivityForResult(intent, REQUEST_CODE_CONNECTING);
    }

    /**
     * Open the activity for editing the selected desktop name
     * @param actionType The type of editing(editing, adding new or deleting)
     */
    private void openEditDesktopNameActivity(final int actionType) /**/{
//		refreshDesktopsDatasList = true;

        final Intent intent = new Intent(context, EditDesktopDataActivity.class);

        intent.putExtra(FILE_NAME, NAMES_DATA_FILE_NAME);

        if(actionType == CREATE_DESKTOP_NAME) {
            Log.d(TAG, "Open the activity for creating a new one");
            intent.putExtra(DESKTOPS_DATA, new WifiDesktopData());
            curEditedDesktopNameInd = NO_EDIT;
        }
        else {
            String name;
            try {
                name = desktopsNames.getSelectedItem().toString();
            } catch (NullPointerException e) {
                Log.e(TAG, "openEditDesktopNameActivity(): the selected item from the desktops names is NULL");
                return;
            }
            intent.putExtra(DESKTOPS_DATA, getDesktopDataByName(name));
            intent.putExtra(IS_DEFAULT, isDesktopDataDefault(name));
            Log.d(TAG, "Open the activity for editing a desktop name " + name);
            curEditedDesktopNameInd = desktopsNames.getSelectedItemPosition();
        }
        activity.startActivityForResult(intent, REQUEST_CODE_EDIT);
    }

    /**
     * Initialization of the button 'New'
     */
    @Override
    protected void initNewButt() {
        buttNew = (Button) activity.findViewById(R.id.newButt);
        buttNew.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                openEditDesktopNameActivity(CREATE_DESKTOP_NAME);
            }
        });
    }

    /**
     * Initialize desktops data from file
     */
    @Override
    public void initDesktopsDatasFromFile() {
        initDesktopsDatasFromFile(NAMES_DATA_FILE_NAME);
    }

    /**
     * Save desktops data to file
     */
    @Override
    public void saveDesktopsDatas() {
        saveDesktopsDatas(NAMES_DATA_FILE_NAME);
    }

    /**
     * Change the currently edited desktop data
     * @param dataStr The string of a new desktop data
     * @param setDefault true The being changed desktop should be signed as default
     */
    private void chgCurrentlyEditedDesktopData(final String dataStr, final boolean setDefault) {
        if (TextUtils.isEmpty(dataStr)) {
            Log.e(TAG, "The given parameter 'dataStr' is NULL or EMPTY");
            return;
        }

        if(curEditedDesktopNameInd == NO_EDIT) {
            Log.d(TAG, "Adding the new desktop data: " + dataStr);
        }
        else {
            Log.d(TAG, "Changing the desktop data: " + dataStr);
            desktopsDatas.remove(curEditedDesktopNameInd);
        }
        if(setDefault) {
            Log.d(TAG, "\tSet the desktop name default");
            desktopsDatas.add(0, new WifiDesktopData(dataStr));
            curSelectedDesktopNameInd = 0;
        } else {
            desktopsDatas.add(new WifiDesktopData(dataStr));
            curSelectedDesktopNameInd = desktopsDatas.size() - 1;
        }
        saveDesktopsDatas();
        curEditedDesktopNameInd = NO_EDIT;
    }

    @Override
    public void processActivityResult(final int requestCode, final Intent data) {
        if (requestCode == UI.REQUEST_CODE_EDIT) {
            final int action_type = data.getIntExtra(EditDesktopDataActivity.ACTION_TYPE, EditDesktopDataActivity.ACTION_ERR);
            switch (action_type) {
                case EditDesktopDataActivity.ACTION_EDIT:
                    chgCurrentlyEditedDesktopData(data.getStringExtra(SINGLE_DESKTOP_DATA), data.getBooleanExtra(IS_DEFAULT, false));
                    host_connection_checked = false;
                    break;
                case EditDesktopDataActivity.ACTION_DEL:
                    removeCurrentlyEditedDesktopData();
                    saveDesktopsDatas();
                    host_connection_checked = false;
                    break;
                case EditDesktopDataActivity.ACTION_CANCEL:
                    host_connection_checked = false;
                    redraw = true;
                    curEditedDesktopNameInd = NO_EDIT;
                    break;
                case EditDesktopDataActivity.ACTION_ERR:
                    Log.e(TAG, "There is some error at transferring an action type from the activity '" + EditDesktopDataActivity.class.getName() + "'");
                    host_connection_checked = true;
                    break;
                default:
                    Log.e(TAG, "Unknown action type " + action_type + " has been received from the activity '" + EditDesktopDataActivity.class.getName() + "'");
                    return;
            }
        }

        if (requestCode == REQUEST_CODE_CONNECTING) {
            host_connection_checked = true;
            redrawSoundVolUI(true);
        }
    }

    /**
     * Turning on wifi connection by opening dialog 'Wifi permission request' for user
     */
    @Override
    public void connectToNetByDialog() {
        activity.startActivity(new Intent(WifiManager.ACTION_PICK_WIFI_NETWORK));
    }

    /**
     * Re-initialize the wifi connector
     */
    @Override
    protected void reInitConnector() {
        if(connector != null) {
//            if(connector.isConnectedToDaemon()) {
//                connector.sendEnd();
//            }
            connector.closeConnection();
        }
        connector = new WifiConnector();
    }

    /**
     * Set volume percent of the currently selected desktop if there is network connection
     * and there is connection to the desktop
     */
    @Override
    public void setVolumeUI() {
        final DesktopData curSelectedDesktopData = desktopsDatas.get(curSelectedDesktopNameInd);

        if(!host_connection_checked) {
            openConnectingActivity(curSelectedDesktopData);
            return;
        }

        try {
            if(connector.isConnectedToDaemon()) {
                setVolumeUiElements();
            }
            else {
                showOkDialog("The audio control service of the application isn't running on the host '" + curSelectedDesktopData.getName() + "'");
            }
        }
        catch(Exception e) {
            showOkDialog("Can't connect to the host '" + curSelectedDesktopData.getName() + "'");
        }
    }

    /**
     * Initialisation of the button 'Edit'
     */
    @Override
    protected void initEditButt() {
        buttEdit = (Button) activity.findViewById(R.id.editButt);
        buttEdit.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                if( (desktopsDatas == null) || (desktopsDatas.isEmpty()) )
                    return;
                openEditDesktopNameActivity(EDIT_DESKTOP_NAME);
            }
        });
        buttEdit.setText(R.string.edit);
    }
}

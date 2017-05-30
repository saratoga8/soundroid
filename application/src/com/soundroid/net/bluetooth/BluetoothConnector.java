/**
 * @file
 * Bluetooth connector
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

package com.soundroid.net.bluetooth;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.util.Log;

import com.soundroid.net.NetConnector;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Method;
import java.security.InvalidParameterException;
import java.util.Arrays;
import java.util.Date;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

/**
 * Connector to other bluetooth devices
 */
public class BluetoothConnector  implements NetConnector {
    public static final int RESPONSE_TIME_OUT_SEC = 50;                /**< Time out for a response in seconds */
    private final String TAG = "BluetoothConnector";                   /**< Tag for logs */

    private BluetoothAdapter btAdapter;                                /**< The blue tooth adapter of the current device */

    private List<BluetoothDevice> pairedDevs;                          /**< The list of the paired blue tooth devices */

    private BluetoothSocket mmSocket;                                  /**< The blue tooth socket instance */

    private ConversationThread conversation;                           /**< The thread of conversation by bluetooth */

    private boolean connectedToDaemon = false;                         /**< Connection to the soundroid daemon state */

    private String connectedDeviceName;                                /**< The name of an other blue tooth device connected to the this one */

    /**
     * Constructor
     */
    public BluetoothConnector() {
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        initPairedDevs();
    }

    /**
     * Initialize the paired blue tooth devices
     */
    public void initPairedDevs() {
        if(pairedDevs == null)
            pairedDevs = new LinkedList<BluetoothDevice>();
        else
            pairedDevs.clear();

        Set<BluetoothDevice> pairedDevices = btAdapter.getBondedDevices();

        if (pairedDevices.size() > 0) {
             for (BluetoothDevice device : pairedDevices) {
                 pairedDevs.add(device);
            }
        }
        else
            Log.w(TAG, "No paired devices has found!");
    }

    /**
     * Get the list of the data of the paired desktops
     * @return The list of the paired by blue tooth desktop datas
     */
    public List<BluetoothDesktopData> getPairedDesktopsDatas() {
        List<BluetoothDesktopData> desktopDatas = new LinkedList<BluetoothDesktopData>();
        String desktopDataStr;
        for(BluetoothDevice dev: pairedDevs) {
            desktopDataStr = dev.getName() + ", " + dev.getAddress();
            desktopDatas.add(new BluetoothDesktopData(desktopDataStr));
        }
        return desktopDatas;
    }

    /**
     * Get the current blue tooth adapter instance
     * @return The blue tooth adapter instance
     */
    public BluetoothAdapter getAdapter() { return btAdapter; }

    /**
     * Is the bluetooth adapter enabled
     * @param context The application context instance
     * @return true if the adapter is enabled
     */
    @Override
    public boolean isConnectedToNet(final Context context) {
        return (btAdapter == null) ? false: btAdapter.isEnabled();
    }

    /**
     * Connect to the remote bluetooth device with the given name
     * @param name The name of the remote device
     * @param port Not used
     * @throws IOException
     */
    @Override
    public void connect(String name, String port) throws IOException {
        final int USED_CHANNEL_NUM = 11;

        try {
            final BluetoothDevice device = getFoundDeviceByName(name);
            Method m = device.getClass().getMethod("createInsecureRfcommSocket", new Class[]{int.class});

            mmSocket = (BluetoothSocket) m.invoke(device, USED_CHANNEL_NUM);
        }
        catch (Exception e) {
            throw new IOException(e.toString());
        }
        mmSocket.connect();
        conversation = new ConversationThread(mmSocket);
        conversation.start();
        connectedDeviceName = name;
    }

    /**
     * Check availability of the given address
     * @param addr The address string
     * @return true The address is reachable
     */
    @Override
    public boolean isAddrReachable(String addr) {
        throw new InvalidParameterException("The method isAddrReachable() hasn't implemented");
    }

    /**
     * Get the found bluetooth device instance by name
     * @param name The name of device
     * @return The instance of the found device
     * @throws InvalidParameterException
     */
    private BluetoothDevice getFoundDeviceByName(final String name) throws InvalidParameterException {
        for(BluetoothDevice device: pairedDevs) {
            if(device.getName().equals(name)) {
                return device;
            }
        }
        throw new InvalidParameterException("There is no found device with the name: " + name);
    }

    /**
     * Get current volume of the remote soundroid daemon
     * @return The string of the volume value
     */
    @Override
    public String getCurVol() {
        final String MSG_STR = "get_vol";
        String answer = "";
        try {
            conversation.write(MSG_STR);
            answer = waitForResponse(5);
        } catch (IOException e) {
            Log.e(TAG, "Can't send " + MSG_STR + " to the remote blue tooth device");
        }
        return answer;
    }

    /**
     * Send the command for mute or unmute the sound's system on the
     * connected desktop
     * @param mute The mute or unmute state should be sent
     * @return true The command was sent and executed successfully
     */
    @Override
    public boolean sendMuteState(final boolean mute) {
        String sentStr = mute ? "mute": "unmute";
        try {
            conversation.write(sentStr);
            String result = waitForResponse(RESPONSE_TIME_OUT_SEC);
            if(result.equalsIgnoreCase("OK"))
                return true;
            else if(result.equalsIgnoreCase("ERR"))
                return false;
            else
                Log.e(TAG, "Unknown received response '" + result + "'");
        } catch (IOException e) {
            Log.e(TAG, "Can't send the command for to the remote device: " + e.toString());
        }
        return false;
    }

    /**
     * Close the connection to a remote device
     */
    @Override
    public void closeConnection() {
        Log.d(TAG, "Close bluetooth connection");
        connectedToDaemon = false;

        if(conversation != null)
            conversation.cancel();
        if(mmSocket != null) {
            try {
                mmSocket.close ();
            } catch (IOException e) {
                Log.e (TAG, "Can't close socket: " + e.toString ());
            }
        }
    }

    /**
     * Send changing volume to the soundroid daemon
     * @param vol The new volume value
     * @return true The sending has done successfully
     */
    @Override
    public boolean sendChgVol(int vol) {
        try {
            conversation.write("chg_vol " + vol);
            return (waitForResponse(RESPONSE_TIME_OUT_SEC).equals("OK"));
        } catch (IOException e) {
            Log.e(TAG, "Can't send command for changing volume at the remote device: " + e.toString());
            return false;
        }
    }

    /**
     * Check the connection to the soundroid daemon
     * @return true Connected to the daemon
     */
    @Override
    public boolean isConnectedToDaemon() {
        return connectedToDaemon;
    }

    /**
     * Send the message HELLO to the remote device and wait for the response HELLO
     * @return true The response is HELLO
     */
    @Override
    public boolean handShake() {
        try {
            conversation.write(HELLO);
        } catch (IOException e) {
            Log.e(TAG, "Can't send " + HELLO + " to the remote blue tooth device");
            return false;
        }

        String answer = waitForResponse (RESPONSE_TIME_OUT_SEC);
        connectedToDaemon = answer.equals(HELLO);
        return connectedToDaemon;
    }

    /**
     * Get the name of the connected blue tooth device
     * @return The device's name
     */
    public String getConnectedDeviceName() {
        return (connectedToDaemon) ? connectedDeviceName: "";
    }

    /**
     * Wait for response from the remote device
     * @param timeOutSeconds The time out of waiting
     * @return The string of response
     */
    private String waitForResponse(final int timeOutSeconds) {
        final long before = (new Date ()).getTime ();
        while(conversation.getReadStr().length() == 0) {
            final long periodSeconds = ((new Date()).getTime() - before) / 1000;
            if(periodSeconds > timeOutSeconds) {
                Log.e(TAG, "Too long awaiting for response(> " + timeOutSeconds + " seconds)");
                break;
            }
        }
        String responseStr = conversation.getReadStr();
        conversation.setReadStr("");
        return responseStr.trim();
    }

    /**
     * Is the remote soundroid daemon muted
     * @return true The daemon is muted
     */
    @Override
    public boolean isMuted() {
        final String MSG_STR = "is_muted";
        try {
            conversation.write(MSG_STR);
        } catch (IOException e) {
            Log.e(TAG, "Can't send " + MSG_STR + " to the remote blue tooth device");
            return false;
        }

        String answer = waitForResponse (RESPONSE_TIME_OUT_SEC);
        return answer.equals("true");
    }

    /**
     * Get the MAC address of the remote bluetooth device with the given name
     * @param name The name of device
     * @return The MAC address of the device or the empty string if hasn't found
     */
    public String getDeviceAddrByName(String name) {
        for(BluetoothDevice dev: pairedDevs) {
            if(dev.getName().equals(name)) {
                return dev.getAddress().replaceAll(" ", "");
            }
        }
        return "";
    }

    /**
     * The thread of the conversation between the local bluetooth adapter and a remote device
     */
    private final class ConversationThread extends Thread {
        private OutputStream sendStream;                          /**< The stream of sending data */
        private InputStream receiveStream;                        /**< The stream of receiving data */

        private byte[] buff;                                      /**< The buffer for data */

        private boolean stopped = true;                           /**< Conversation stopped */
        private String readStr = "";                              /**< The read string */

        /**
         * Constructor
         * @param socket The bluetooth socket instance
         * @throws IOException
         */
        public ConversationThread(final BluetoothSocket socket) throws IOException {
            sendStream = socket.getOutputStream();
            receiveStream = socket.getInputStream();

            buff = new byte[100];
        }

        /**
         * Write the given string to the stream of conversation
         * @param str The string
         * @throws IOException
         */
        public void write(final String str) throws IOException {
            Log.d(TAG, "Sending " + str);
            sendStream.write(str.getBytes());
            sendStream.flush();
        }

        /**
         * Run the thread
         */
        public void run () {
            stopped = false;
            read();
        }

        /**
         * Is the conversation stopped
         * @return true if stopped
         */
        private synchronized boolean isStopped() {
            return stopped;
        }

        /**
         * Set the conversation stopped
         */
        private synchronized void setStopped() {
            stopped = true;
        }

        /**
         * Get the string read from the conversation stream
         * @return The string
         */
        public synchronized String getReadStr() {
            return readStr;
        }

        /**
         * Set the string read from the conversation stream
         * @param str The string
         */
        private synchronized void setReadStr(final String str) {
            readStr = str;
        }

        /**
         * Read from the conversation stream
         */
        private void read () {
            int bytes;
            while (!isStopped()) {
                try {
                    bytes = receiveStream.read(buff);
                    if(bytes > 0) {
                        byte responseByteArr[] = new byte[bytes];
                        System.arraycopy(buff, 0, responseByteArr, 0, bytes);
                        String response = new String(responseByteArr, "UTF-8");
                        Log.d(TAG, "Response: " + response + ". Length: " + response.length());
                        Arrays.fill(buff, (byte) 0);
                        setReadStr(response);
                    }
                } catch (IOException e) {
                    Log.e(TAG, "Can't read from the InputStream: " + e.toString());
                    break;
                }
            }
        }

        /**
         * Cancel the conversation
         */
        public void cancel() {
            try {
                setStopped();
                while(!isStopped());
                sendStream.close();
            } catch (IOException e) {
                Log.e(TAG, "Can't close the output stream: " + e.toString());
            }
            try {
                receiveStream.close();
            } catch (IOException e) {
                Log.e(TAG, "Can't close the input stream: " + e.toString());
            }
        }
    }
}

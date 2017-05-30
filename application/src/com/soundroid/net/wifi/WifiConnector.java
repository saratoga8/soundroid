/**
 * @file
 * Network connector for connecting to the given IP address and port
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

package com.soundroid.net.wifi;

import android.content.Context;
import android.net.wifi.WifiManager;
import android.util.Log;

import com.soundroid.net.NetConnector;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;

/**
 * Wifi Network connector initializes connection to the given IP address and port
 * sends and receives given strings by the established connection
 */
public class WifiConnector implements NetConnector {
    private static final int TIME_OUT = 10000;               /**< Connection's time out in milliseconds */

	private String TAG = "DistanceVolume.WifiConnector";      /**< The tag for writing to log */
	
	private Socket socket = null;                            /**< The socket's instance */
	
	private OutputStream outStream = null;                   /**< The output stream instance */
	private DataOutputStream dataOutStream = null;           /**< The data output stream instance */

	private InputStream inStream = null;                     /**< The input stream instance */
	private DataInputStream dataInStream = null;             /**< The data input stream instance */

	private InetSocketAddress addr;                          /**< The network address for connecting */
	private String macAddrStr = "";                          /**< The string of a MAC address */ 

	private boolean connectedToDaemon = false;               /**< Is the connection to daemon established? */
	
	/**
	 * Constructor
	 */
	public WifiConnector() {
		socket = new Socket();
	}

	/**
	 * Return connection to a network
	 * @param context The application's context
	 * @return true Connected to a network
	 */
	@Override
    public boolean isConnectedToNet(final Context context) {
        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        return (wifiManager.isWifiEnabled());
    }

    /**
	 * Connects to the given IP and port
	 * @param ip The IP for connecting
	 * @param port The port number 
	 * @throws IOException
	 */
	@Override
	public void connect(final String ip, final String port) throws IOException {
		Log.d(TAG, "Init socket to address: " + ip + " with the port: " + port);

		connectedToDaemon = false;
		
		macAddrStr = ip;		

		addr = new InetSocketAddress(macAddrStr, Integer.parseInt(port));

		if(socket.isClosed())
			socket = new Socket();

		socket.connect(addr);
		socket.setSoTimeout(TIME_OUT);
	}

	/**
	 * Checks is the given IP reachable by network
	 * @param ip The IP for checking 
	 * @return true The IP is reachable
	 */
	@Override
	public boolean isAddrReachable(final String ip) {
		try {
			InetAddress address = InetAddress.getByName(ip);
			return address.isReachable(TIME_OUT);
		}
		catch(IOException e) {
			Log.d(TAG, "In isAddrReachable(): " + e.toString());
			return false;
		}
		catch(SecurityException e) {
			Log.d(TAG, "In isAddrReachable(): " + e.toString());
			return false;
		}
		
	}
	
	/**
	 * Get the current sound's volume on the connected desktop  
	 * @return The string of the sound's volume in percents
	 */
	@Override
	public String getCurVol() {
		String volStr = "";
		try {
			sendStr("get_vol");
			volStr = receiveStr();
		} catch (IOException e) {
			Log.e(TAG, "Can't get current volume from the IP=" + macAddrStr + ": " + e.toString());
		}
		
		return volStr;
	}
	
//	private void reConnect() throws IOException {
//		if(socket.isClosed() || !socket.isConnected()) {
//			Log.d(TAG, "Reconnection to " + macAddrStr + ":" + port);
//			socket = new Socket(macAddrStr, port);
//		}
//	}
	
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
			sendStr(sentStr);
			String result = receiveStr(); 
			if(result.equalsIgnoreCase("OK"))
				return true;
			else if(result.equalsIgnoreCase("ERR"))
				return false;
			else
				Log.e(TAG, "The received string is '" + result + "'");
		} catch (IOException e) {
			Log.e(TAG, "Can't send the command for " + sentStr + " to the IP=" + macAddrStr + ": " + e.toString());
		}
		return false;
	}
	
	/**
	 * Send the command for end connection
	 */
//	public void sendEnd() {
//		try {
//			sendStr("end");
//		} catch (IOException e) {
//			Log.e(TAG, "Can't end session with the IP=" + macAddrStr + ": " + e.toString());
//		}
//        connectedToDaemon = false;
//	}
	
	/**
	 * Send the given string to the connected desktop
	 * @param str The string for sending
	 * @throws IOException
	 */
	private void sendStr(final String str) throws IOException {
		Log.d(TAG, "Sending '" + str + "' by socket");		
		
		outStream = socket.getOutputStream();
		dataOutStream = new DataOutputStream(outStream);
		dataOutStream.writeBytes(str + "\n\r");
	}

	/**
	 * Close the connection to the connected desktop
	 */
	@Override
	public void closeConnection() {
		Log.d(TAG, "Close connection");

		connectedToDaemon = false;

		try {
			if(socket != null) {
				socket.close();
			}
		}
		catch (IOException e) {
			Log.e(TAG, "Can't close socket: " + e.toString());
		}		
		try {
        	if(dataOutStream != null) {
        		dataOutStream.close();
        		dataOutStream = null;
        	}
		}
    	catch (IOException e) {
    		Log.e(TAG, "Can't close data output stream: " + e.toString());
    	}
        try {
        	if(outStream != null) {
        		outStream.close();
        		outStream = null;
        	}
		}
    	catch (IOException e) {
    		Log.e(TAG, "Can't close output stream: " + e.toString());
    	}
        	
        try {
        	if(dataInStream != null) {
				dataInStream.close();
				dataInStream = null;
        	}
		}
        catch (IOException e) {
        	Log.e(TAG, "Can't close data input stream: " + e.toString());
       	}
        try {
			if(inStream != null) {
				inStream.close();
				inStream = null;
			}
		}
		catch (IOException e) {
			Log.e(TAG, "Can't close input stream: " + e.toString());
		}
	}
	
	/**
	 * Receive a string from the connected desktop 
	 * @return The received string 
	 * @throws IOException
	 */
	private String receiveStr() throws IOException {
		inStream = socket.getInputStream();
		dataInStream = new DataInputStream(inStream);

		String str = dataInStream.readLine();
		Log.d(TAG, "The received string is '" + str + "'");
			
		return str;
	}
	
	/**
	 * Send the command for changing sound's volume at the connected desktop
	 * to the given percent
	 * @param vol The volume in percents
	 * @return true The volume has been changed successfully
	 */
	@Override
	public boolean sendChgVol(final int vol) {
		try {
			sendStr("chg_vol " + vol);
			return (receiveStr().equals("OK"));
		} catch (IOException e) {
			Log.e(TAG, "Can't change volume to " + vol + " at the IP=" + macAddrStr + ": " + e.toString());
			return false;
		}
	}
	
	/**
	 * Is the connection to the remote desktop established?
	 * @return true The connection established
	 */
	@Override
	public boolean isConnectedToDaemon() {
		return connectedToDaemon;
	}
	
	/**
	 * Send hello to the connected desktop and get hello from it
	 * @return true The handshake has made successfully
	 */
	@Override
	public boolean handShake() {
		connectedToDaemon = false;
		try {
			sendStr(HELLO);
			String answerStr = receiveStr();
			if(!answerStr.equals(HELLO)) {
				Log.e(TAG, "The answer of the handshake with the IP=" + macAddrStr + " is '" + answerStr + "'");
				return false;
			}
		} catch (IOException e) {
			Log.e(TAG, "Can't make hand shake with the IP=" + macAddrStr + ": " + e.toString());
			return false;
		}
		return connectedToDaemon = true;
	}

	/**
	 * Is the sound volume of the connected desktop muted? 
	 * @return true The desktop muted
	 */
	@Override
	public boolean isMuted() {
		try {
			sendStr("is_muted");
			String result = receiveStr(); 
			if(result.equalsIgnoreCase("true"))
				return true;
			else if(result.equalsIgnoreCase("false"))
				return false;
			else
				Log.e(TAG, "The received string is '" + result + "'");
		} catch (IOException e) {
			Log.e(TAG, "Can't check the mute state at the IP=" + macAddrStr + ": " + e.toString());
		}
		return false;
	}
}

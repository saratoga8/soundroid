/**
 * @file
 * Connector to a network(e.g. by wifi or bluetooth)
 * The desktop data is the data about the computer the application connects to for changing sound volume
 * of the desktop.
 * The desktop data consists of the desktop's IP, the port number of the daemon running on the desktop
 * and the desktop's name, given by user.
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
package com.soundroid.net;

import android.content.Context;

import java.io.IOException;

/**
 * Network connector
 */
public interface NetConnector {
    String HELLO = "hello";                /**< String constant 'hello' */

    /**
     * Connection types, e.g. wifi, bluetooth
     */
    enum ConnectionTypes { WIFI, BLUE_TOOTH }

    /**
     * Return connection to a network
     * @param context The application's context
     * @return true Connected to a network
     */
    boolean isConnectedToNet   (final Context context);

    /**
     * Connect to the given address and port
     * @param addr The address for connection
     * @param port The port for connection
     * @throws IOException
     */
    void    connect            (final String addr, final String port) throws IOException;

    /**
     * Check availability of the given address
     * @param addr The address string
     * @return true The address is reachable
     */
    boolean isAddrReachable    (final String addr);

    /**
     * Get current volume of the remote soundroid daemon
     * @return The string of the volume value
     */
    String  getCurVol          ();

    /**
     * Send the state of mute to the remote soundroid daemon
     * @param mute The state of mute: true - to mute, false - to unmute
     * @return true The sending has done successfully
     */
    boolean sendMuteState      (final boolean mute);

    /**
     * Close the connection to a network
     */
    void    closeConnection    ();

    /**
     * Send changing volume to the soundroid daemon
     * @param vol The new volume value
     * @return true The sending has done successfully
     */
    boolean sendChgVol         (final int vol);

    /**
     * Check the connection to the soundroid daemon
     * @return true Connected to the daemon
     */
    boolean isConnectedToDaemon();

    /**
     * Make hand shake with the soundroid daemon
     * @return true The handshake has done successfully
     */
    boolean handShake          ();

    /**
     * Is the remote soundroid daemon muted
     * @return true The daemon is muted
     */
    boolean isMuted            ();
}

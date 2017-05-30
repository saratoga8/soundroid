/**
 * @file
 * The data of a desktop connected by wifi
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

import android.text.TextUtils;

import com.soundroid.net.DesktopData;

import java.security.InvalidParameterException;

/**
 * Created by saratoga on 10/10/14.
 */
public class WifiDesktopData extends DesktopData {
    private static final int IP_ADDR_PARTS_NUM = 4;

    /**
     * Constructor initializing desktop's data
     * @param name The name of the desktop
     * @param address The desktop's IP
     * @param port The port number of the daemon on the desktop
     */
    public WifiDesktopData(final String name, final String address, final String port) {
        if(TextUtils.isEmpty(name)) {
            throw new InvalidParameterException("The given desktop's name is NULL or EMPTY");
        }
        if(TextUtils.isEmpty(address)) {
            throw new InvalidParameterException("The given desktop's address is NULL or EMPTY");
        }
        if(TextUtils.isEmpty(port)) {
            throw new InvalidParameterException("The given desktop's port is NULL or EMPTY");
        }

        this.name = name;

        chkAddress(address);
        this.address = address;

        try {
            Integer.parseInt(port);
            this.port = port;
        }
        catch (NumberFormatException e) {
            throw new InvalidParameterException("The given port '" + port + "' doesn't contain number");
        }
    }

    /**
     * Default desktop
     * Make all data empty
     */
    public WifiDesktopData() {
        setName("");
        setAddress("");
        setPort("");
    }

    /**
     * Check the given string is IP address
     * @param addrStr The IP string
     */
    private void chkAddress(final String addrStr) {
        String arr[] = addrStr.split("\\.");
        if(arr.length != 4) {
            throw new InvalidParameterException("The size of the IP address " + addrStr + " is not 4 numbers");
        }
        for(String numStr: arr) {
            try {
                final int num = Integer.parseInt(numStr);
                if(num > 255) {
                    throw new InvalidParameterException("The IP address contains number " + numStr + " which is more than 255");
                }
            }
            catch (NumberFormatException e) {
                throw new InvalidParameterException("The IP address " + numStr + " contains non-number characters");
            }
        }
    }

    /**
     * Constructor initializing a desktop data from the given string
     * @param data The string with desktop's data
     */
    public WifiDesktopData(final String data) {
        if(TextUtils.isEmpty(data)) {
            throw new InvalidParameterException("The given desktop's data string is NULL or EMPTY");
        }

        String arr[] = data.split(",");
        try {
            name = arr[0];
        }
        catch (ArrayIndexOutOfBoundsException e) {
            throw new InvalidParameterException("The given desktop's data string '" + data + "' doesn't contain the desktop's name");
        }
        try {
            address = arr[1];
        }
        catch (ArrayIndexOutOfBoundsException e) {
            throw new InvalidParameterException("The given desktop's data string '" + data + "' doesn't contain the desktop's IP");
        }
        try {
            port = arr[2];
        }
        catch (ArrayIndexOutOfBoundsException e) {
            throw new InvalidParameterException("The given desktop's data string '" + data + "' doesn't contain the desktop's port");
        }
    }


    /**
     * Get desktop's data in string
     * @return The string of the desktop's data
     */
    @Override
    public String toString() {
        return name + "," + address + "," + port;
    }

    /**
     * Get the IP as a strings array
     * @return The array of strings
     */
    @Override
    public String[] getAddressArr() {
        return getAddressArr(IP_ADDR_PARTS_NUM, "\\.");
    }

    /**
     * Compare the data with an other one
     * @param otherData The other blue tooth desktop data
     * @return true The datas are equal
     */
    @Override
    public boolean equals(final Object otherData) {
        String otherName = ((WifiDesktopData)otherData).getName();
        String otherAddr = ((WifiDesktopData)otherData).getAddress();
        String otherPort = ((WifiDesktopData)otherData).getPort();

        return (otherAddr.equals(address) && otherName.equals(name) && otherPort.equals(port));
    }
}

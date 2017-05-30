/**
 * @file
 * The data of a desktop connected by blue tooth
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

import android.text.TextUtils;
import android.util.Log;

import com.soundroid.net.DesktopData;

import java.security.InvalidParameterException;

/**
 * Created by saratoga on 10/10/14.
 */
public class BluetoothDesktopData extends DesktopData {

    private static final String TAG = "BluetoothDesktopData";                 /**< Tag for logs */
    private static final int BT_MAC_ADDR_PARTS_NUM = 6;                       /**< The number of the parts of a blue tooth address */

    /**
     * Constructor from the given string
     * @param dataStr The data string
     */
    public BluetoothDesktopData(final String dataStr) {
        if(TextUtils.isEmpty(dataStr)) {
            throw new InvalidParameterException("The given desktop data string is NULL or EMPTY");
        }

        String arr[] = dataStr.split("\\s*,\\s*");
        try {
            name = arr[0];
        }
        catch (ArrayIndexOutOfBoundsException e) {
            throw new InvalidParameterException("The given desktop data string '" + dataStr + "' doesn't contain the desktop name");
        }
        try {
            address = arr[1];
        }
        catch (ArrayIndexOutOfBoundsException e) {
            throw new InvalidParameterException("The given desktop data string '" + dataStr + "' doesn't contain the desktop MAC address");
        }
    }

    /**
     * Constructor from a desktop's name and its address
     * @param name The name
     * @param addr The address string
     */
    public BluetoothDesktopData(final String name, final String addr) {
        if(TextUtils.isEmpty(name)) {
            throw new InvalidParameterException("The given bluetooth desktop name is NULL or EMPTY");
        }
        this.name = name;
        if(TextUtils.isEmpty(addr)) {
            throw new InvalidParameterException("The given bluetooth desktop address is NULL or EMPTY");
        }
        this.address = addr;
    }

    /**
     * Get the address array of strings
     * @return The address array of strings
     */
    @Override
    public String[] getAddressArr() {
        if(address.contains(":"))
            return getAddressArr(BT_MAC_ADDR_PARTS_NUM, ":");
        else if (address.contains("-"))
            return getAddressArr(BT_MAC_ADDR_PARTS_NUM, "-");

        Log.e(TAG, "Unknown delimiter character used in the Bluetooth MAC address");
        return new String[BT_MAC_ADDR_PARTS_NUM];
    }

    /**
     * Get the string of the blue tooth desktop data
     * @return The string of the data
     */
    @Override
    public String toString() {
        return name + "," + address;
    }

    /**
     * Compare the data with an other one
     * @param otherData The other blue tooth desktop data
     * @return true The datas are equal
     */
    @Override
    public boolean equals(final Object otherData) {
        final String otherName = ((BluetoothDesktopData)otherData).getName();
        final String otherAddr = ((BluetoothDesktopData)otherData).getAddress();
        return (otherAddr.equals(address)) && (otherName.equals(name));
    }
}

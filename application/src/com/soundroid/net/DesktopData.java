/**
 * @file
 * A desktop data.
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

import java.io.Serializable;

/**
 * The data of the desktop and operations with it 
 */
public abstract class DesktopData implements Serializable {
	protected static final long serialVersionUID = 1L;
	
	protected String name;     /**< The desktop's name, given by user*/
    protected String address;  /**< The IP of the desktop */
    protected String port;     /**< The port number of the daemon, running on the desktop */

	/**
	 * Get desktop's name
	 * @return The desktop's name
	 */
	public String getName() {
		return name;
	}
	
	/**
	 * Get desktop's IP 
	 * @return The desktop's IP
	 */
	public String getAddress() {
		return address;
	}
	
	/**
	 * Get the port number of the daemon running on the desktop
	 * @return The port number
	 */
	public String getPort() {
		return port;
	}

	/**
	 * Set desktop's name
	 * @param name The desktop's new name
	 */
	public void setName(String name) {
		if (name == null) {
			throw new NullPointerException("The given parameter name is NULL");
		}
		this.name = name;
	}

	/**
	 * Set desktop's IP
	 * @param address The desktop's new IP
	 */
	public void setAddress(String address) {
		if (address == null) {
			throw new NullPointerException("The given parameter address is NULL");
		}
		this.address = address;
	}

	/**
	 * Set the desktop's port number 
	 * @param port The desktop's new port
	 */
	public void setPort(String port) {
		if (port == null) {
			throw new NullPointerException("The given parameter port is NULL");
		}
		this.port = port;
	}

	/**
	 * Get the address array of strings
	 * @param len The length of the address
	 * @param delimiter
	 * @return The address array of strings
	 */
    protected String[] getAddressArr(final int len, final String delimiter) {
        if(address.length() == 0) {
            return new String[len];
        }

        return address.split(delimiter);
    }

	/**
	 * Get the address array of strings
	 * @return The address array of strings
	 */
    public abstract String[] getAddressArr();

	/**
	 * Is the desktop's data empty?
	 * @return true The data is empty
	 */
	public boolean isEmpty() {
		if(name.length() == 0) {
			if(address.length() == 0) {
				return (port.length() == 0);
			}
		}
		return false;
	}
}

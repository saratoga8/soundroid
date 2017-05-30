/**
 * @file
 * Utils for work with files
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

package com.soundroid.others;

import android.content.Context;
import android.text.TextUtils;

import com.soundroid.net.DesktopData;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.security.InvalidParameterException;
import java.util.ArrayList;
import java.util.List;

/**
 * Utils for work with files
 */
public abstract class FileUtils {
	
	/**
	 * Get one string from the file with the given name
	 * @param context The app's context instance
	 * @param fileName The file name for reading
	 * @return The read string from the file
	 * @throws IOException
	 */
	public static String getOneLineFromFile(final Context context, String fileName) throws IOException {
		if (TextUtils.isEmpty(fileName)) {
			throw new NullPointerException("The given parameter 'fileName' is NULL or EMPTY");
		}
		
		FileInputStream inputStream = null;
		BufferedReader bufferedReader = null;
		try {
			inputStream  = context.openFileInput(fileName);
			bufferedReader = new BufferedReader(new InputStreamReader(inputStream));

			String line = bufferedReader.readLine();

			return (line != null) ? line: "";
			
		} catch (IOException e) {
			throw new IOException("Can't read the file " + fileName + ": " + e.toString());
		}
		finally {
			closeBufferReaderAndInputStream(inputStream, bufferedReader);
		}
	}

	/**
	 * Close the buffer of reader and input stream
	 * @param inputStream The input stream instance
	 * @param bufferedReader The buffer reader instance
	 * @throws IOException
	 */
	private static void closeBufferReaderAndInputStream(final FileInputStream inputStream, final BufferedReader bufferedReader)	throws IOException {
		if(bufferedReader != null) {
			try {
				bufferedReader.close();
			} catch (IOException e) {
				throw new IOException("Can't close the buffer reader instance: " + e.toString());
			}
		}
		if(inputStream != null) {
			try {
				inputStream.close();
			} catch (IOException e) {
				throw new IOException("Can't close the file input stream instance: " + e.toString());
			}
		}
	}
	
	/**
	 * Get lines from the file with the given name
	 * @param context The app's context
	 * @param fileName The name of the file
	 * @return The array txt_view_item of the string from the file
	 * @throws IOException
	 */
	public static ArrayList<String> getLinesFromFile(final Context context, String fileName) throws IOException {
		if (TextUtils.isEmpty(fileName)) {
			throw new InvalidParameterException ("The given parameter 'fileName' is NULL or EMPTY");
		}
		
		FileInputStream inputStream = null;
		BufferedReader bufferedReader = null;

		try {
			inputStream  = context.openFileInput(fileName);
			bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
			String line;
			ArrayList<String> names = new ArrayList<String>();  
			while( (line = bufferedReader.readLine()) != null ) {
				names.add(line);
			}
			return names;
			
		} catch (IOException e) {
			throw new IOException("Can't read the file " + fileName + ": " + e.toString());
		}
		finally {
			closeBufferReaderAndInputStream(inputStream, bufferedReader);
		}
	}
	
	/**
	 * Save the given data string to the file with the given name
	 * @param context The app's context
	 * @param fileName The file name
	 * @param data The data string 
	 * @param mode The mode of saving(adding or rewriting)
	 * @throws IOException
	 */
	public static void saveDataStr(final Context context, String fileName, String data, final int mode) throws IOException {
		if (TextUtils.isEmpty(fileName)) {
			throw new NullPointerException("The given parameter 'fileName' is NULL or EMPTY");
		}
		
		if(TextUtils.isEmpty(data)) {
			return;
		}
		
		FileOutputStream outputStream = null;
		try {
			outputStream  = context.openFileOutput(fileName, mode);
			outputStream.write(data.getBytes());
		} catch (Exception e) {
			throw new IOException("Cant write to the file " + fileName + ": " + e.toString());
		}
		finally {
			closeOutputStream(outputStream);
		}
	}
	
	/**
	 * Save data txt_view_item to the file with the given name
	 * @param context The app's context
	 * @param fileName The file name
	 * @param data The txt_view_item of the data for saving
	 * @throws IOException
	 */
	public static void saveDataList(final Context context, String fileName, final List<DesktopData> data) throws IOException {
		if (TextUtils.isEmpty(fileName)) {
			throw new NullPointerException("The given parameter 'fileName' is NULL or EMPTY");
		}
		
		if (data == null) {
			throw new NullPointerException("The given parameter 'data' is NULL");
		}

		StringBuilder txt = new StringBuilder();
		for(DesktopData curDesktopData: data) {
			txt = txt.append((curDesktopData.toString() + "\n"));
		}
		
		FileOutputStream outputStream = null;
		try {
			outputStream  = context.openFileOutput(fileName, Context.MODE_PRIVATE);
			outputStream.write(txt.toString().getBytes());
		} catch (Exception e) {
			throw new IOException("Cant write to the file " + fileName + ": " + e.toString());
		}
		finally {
			closeOutputStream(outputStream);
		}
	}
	
	/**
	 * Close the output stream
	 * @param outputStream The output stream instance
	 * @throws IOException
	 */
	private static void closeOutputStream(OutputStream outputStream) throws IOException {
		if(outputStream != null) {
			try {
				outputStream.close();
			} catch (IOException e) {
				throw new IOException("Can't close the file output stream instance: " + e.toString());
			}
		}
	}
}

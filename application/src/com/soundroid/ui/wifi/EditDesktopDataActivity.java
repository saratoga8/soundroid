/**
 * @file
 * Activity for editing a desktop's data
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
import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Toast;

import com.soundroid.R;
import com.soundroid.net.DesktopData;
import com.soundroid.net.wifi.WifiDesktopData;
import com.soundroid.others.FileUtils;
import com.soundroid.ui.UI;

import java.io.IOException;
import java.security.InvalidParameterException;
import java.util.ArrayList;


/**
 * Activity for editing a desktop's data
 */
public class EditDesktopDataActivity extends Activity {
	private static final String TAG = "EditDesktopNameActivity";                                  /**< The tag for writing to log */ 

	public final static String ACTION_TYPE = "ACTION_TYPE";
	public final static int ACTION_DEL = 1, ACTION_EDIT = 2, ACTION_ERR = 3, ACTION_CANCEL = 4;   /**< Edit action types */

	private EditText ipEditTxtArr[];                                                              /**< The array of edit text fields for editing IP */
	private EditText portEditTxt;                                                                 /**< The field for editing port number */
	private EditText nameEditTxt;                                                                 /**< The field for editing desktop's name */
	 
	private CheckBox defaultChkBox;                                                               /**< The check box for checking the currently edited desktop as the first
	                                                                                               * within the drop box of desktop names at the main activity */
	private Button buttOK, buttDel, buttCancel;                                                   /**< Buttons OK, Delete, Cancel */
	
	private ArrayList<String> existingDesktopsDatasStrs;                                          /**< The string, containing the data about the known desktops */
	
	private DesktopData editedDesktopData = null;                                                 /**< The desktop data should be edited, and given from the main activity */
	private final int NO_INDEX = -1;                                                              /**< No index selected in the drop box */
	private int editedDesktopDataIndex = NO_INDEX;                                                /**< The index of the currently selected desktop data from the drop box
	                                                                                              /* of the main activity */
	         
	/**
	 * Getting the desktop data should be edited from the main activity
	 * Initializing UI 
	 */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.edit_desktop_name);
        
        final Intent intent = getIntent();
        editedDesktopData = (DesktopData)intent.getSerializableExtra(UI.DESKTOPS_DATA);
        
        final boolean isDefault = intent.getBooleanExtra(UI.IS_DEFAULT, false);
        if(editedDesktopData == null) {
        	Log.e(TAG, "Can't get desktop data from the intent of the main activity");
        	editedDesktopData = new WifiDesktopData();
        }
        
        initUI(editedDesktopData, isDefault);

        final String fileName = intent.getStringExtra(UI.FILE_NAME);
        try {
			existingDesktopsDatasStrs = FileUtils.getLinesFromFile(this, fileName);
		} catch (IOException e) {
			Log.e(TAG, "Can't get the existing desktops from the file " + fileName + ": " + e.toString());
			existingDesktopsDatasStrs = new ArrayList<String>();
		} 
        
        editedDesktopDataIndex = existingDesktopsDatasStrs.indexOf(editedDesktopData.toString());
    }

    /**
     * Initialization of UI elements by the given desktop data
     * @param desktopData The desktop data should be edited
     * @param isDefault Is the given desktop should be default in the drop box of the main activity?
     */
	private void initUI(final DesktopData desktopData, final boolean isDefault) {
		defaultChkBox = (CheckBox) findViewById(R.id.defaultChkBox);
		defaultChkBox.setChecked(isDefault);

		portEditTxt = (EditText) findViewById(R.id.portEditTxt);
		portEditTxt.setText(desktopData.getPort());
		nameEditTxt = (EditText) findViewById(R.id.nameEditTxt);
		nameEditTxt.setText(desktopData.getName());
		
		initIpEditTxtFields(desktopData.getAddressArr(), portEditTxt);
		
		initButtons();
	}
	
	/**
	 * Initialization of the IP field
	 * @param ip The array of IP address
	 * @param lastFocusedEditBox The edit box will be focused automatically after 
	 * the editing this field 
	 */
	private void initIpEditTxtFields(final String ip[], final EditText lastFocusedEditBox) {
		ipEditTxtArr = new EditText[4];

		ipEditTxtArr[0] = (EditText) findViewById(R.id.ipEditTxt1);
		ipEditTxtArr[1] = (EditText) findViewById(R.id.ipEditTxt2);
		ipEditTxtArr[2] = (EditText) findViewById(R.id.ipEditTxt3);
		ipEditTxtArr[3] = (EditText) findViewById(R.id.ipEditTxt4);
		
		final int IP_SEGMENT_LEN = 3;
		ipEditTxtArr[0].addTextChangedListener(getTxtWatcherForEditTxt(ipEditTxtArr[1], IP_SEGMENT_LEN));
		ipEditTxtArr[1].addTextChangedListener(getTxtWatcherForEditTxt(ipEditTxtArr[2], IP_SEGMENT_LEN));
		ipEditTxtArr[2].addTextChangedListener(getTxtWatcherForEditTxt(ipEditTxtArr[3], IP_SEGMENT_LEN));
		ipEditTxtArr[3].addTextChangedListener(getTxtWatcherForEditTxt(lastFocusedEditBox, IP_SEGMENT_LEN));

		if(ip == null) {
			Log.e(TAG, "The given IP array is NULL");
			return;
		}
		if(ip.length == 0) {
			Log.e(TAG, "The given IP array has size 0");
			return;
		}
		
		for(int i = 0; i < 4; ++i) {
			ipEditTxtArr[i].setText(ip[i]);
		}
	}

	/**
	 * Text watcher for edit text. When the editing within the current edit text has finished
	 * and the cursor within the current edit box is at the last position, the focus will be moved
	 * to the next given edit box
	 * @param editTxtShouldBeFocused The edit text should be focused after the current edit box
	 * @param lastPosBeforeRefocus The last position within the currently edited box
	 * @return The text watcher instance
	 */
	private static TextWatcher getTxtWatcherForEditTxt(final EditText editTxtShouldBeFocused, final int lastPosBeforeRefocus) {
		return new TextWatcher() {
			@Override
			public void afterTextChanged(Editable s) {
				if(s.toString().length() == 3)
					editTxtShouldBeFocused.requestFocus();
	          }	
			
			@Override
			public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
			@Override
			public void onTextChanged(CharSequence s, int start, int before, int count) {}
		};
	}	
	
	/**
	 * Initialize buttons
	 */
	private void initButtons() {
		initOkButt();
		initCancelButt();
		initDelButt();
	}

	/**
	 * Initialize the button OK
	 */
	private void initOkButt() {
		buttOK = (Button) findViewById(R.id.buttOK);
		buttOK.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				Log.d(TAG, "Editing the data of the desktop name " + nameEditTxt.getText().toString());
			
				final Intent intent = new Intent();
				intent.putExtra(ACTION_TYPE, ACTION_EDIT);
				try {
					intent.putExtra(UI.IS_DEFAULT, defaultChkBox.isChecked());
					intent.putExtra(UI.SINGLE_DESKTOP_DATA, getDataFromUI());
					
					existingDesktopsDatasStrs.clear();
					
					setResult(RESULT_OK, intent);
					finish();
				}
				catch (InvalidParameterException e) {
					Toast.makeText(v.getContext(), "ERROR: " + e.getMessage(), Toast.LENGTH_SHORT).show();
				}
			}
		});
	}

	/**
	 * Initialize the button Cancel
	 */	
	private void initCancelButt() {
		buttCancel = (Button) findViewById(R.id.buttCancel);
		buttCancel.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				Log.d(TAG, "Cancel the activity");
				
				existingDesktopsDatasStrs.clear();
				
				final Intent intent = new Intent();
				intent.putExtra(ACTION_TYPE, ACTION_CANCEL);
				setResult(RESULT_OK, intent);
				finish();
			}	
		});
	}

	/**
	 * Initialize the button Delete
	 */
	private void initDelButt() {
		buttDel = (Button) findViewById(R.id.buttDel);
		buttDel.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				Log.d(TAG, "Delete the desktop " + nameEditTxt.getText().toString());
				
				existingDesktopsDatasStrs.clear();
				
				final Intent intent = new Intent();
				intent.putExtra(ACTION_TYPE, ACTION_DEL);
				setResult(RESULT_OK, intent);
				finish();				
			}
		});	
		
		buttDel.setVisibility((editedDesktopData.isEmpty()) ? View.INVISIBLE: View.VISIBLE);  // if it creating a new desktop, there shouldn't be a 'delete' button
	}	
	
	/**
	 * Get data from UI in a string form
	 * @return The string which is the data from UI 
	 */
	private String getDataFromUI() {
		Log.d(TAG, "Get data from UI");
		
		StringBuilder ip = new StringBuilder();
		for (EditText curEditTxt: ipEditTxtArr) {
			String txt = curEditTxt.getText().toString();
			if(TextUtils.isEmpty(txt)) {
				throw new InvalidParameterException("The IP address has not fully entered!");
			}
			if(Integer.parseInt(txt) > 255) {
				throw new InvalidParameterException("In IP address there is number " + txt + " > 255");
			}
			ip = ip.append(txt + ".");
		}
		ip = ip.deleteCharAt(ip.length() - 1);
		String ipStr = ip.toString();
		if(isIpUsed(ipStr)) {
			throw new InvalidParameterException("The IP: " + ipStr + " is used already");
		}
		
		String port = portEditTxt.getText().toString();
		if(TextUtils.isEmpty(port)) {
			throw new InvalidParameterException("The port hasn't entered!");
		}
		
		String name = nameEditTxt.getText().toString();
		if(TextUtils.isEmpty(name)) {
			throw new InvalidParameterException("The name hasn't entered!");
		}
		if(isNameUsed(name)) {
			throw new InvalidParameterException("The name '" + name + "' exists already");
		}
		
		String uiDataStr = name + "," + ipStr + "," + port;
		
		return uiDataStr; 
	}
	
	/**
	 * Check is the given name is already used by some desktop?
	 * @param name The checked name
	 * @return true The name is used
	 */
	private boolean isNameUsed(final String name) {
		if(TextUtils.isEmpty(name)) {
			Log.e(TAG, "The given name is EMPTY or NULL");
		}
				
		for(int i = 0; i < existingDesktopsDatasStrs.size(); ++i) {
			if(i == editedDesktopDataIndex) {
				continue;
			}
			if(existingDesktopsDatasStrs.get(i).startsWith(name)) {
				return true;
			}
		}

		return false;		
	}
	
	/**
	 * Is the given IP is already used by some desktop?
	 * @param ip The checked IP
	 * @return true The IP is used
	 */
	private boolean isIpUsed(final String ip) {
		if(TextUtils.isEmpty(ip)) {
			Log.e(TAG, "The given IP is EMPTY or NULL");
		}
				
		for(int i = 0; i < existingDesktopsDatasStrs.size(); ++i) {
			if(i == editedDesktopDataIndex) {
				continue;
			}
			if(existingDesktopsDatasStrs.get(i).contains(ip)) {
				return true;
			}
		}

		return false;			
	}
}
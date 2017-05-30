/**
 * @file
 * Activity for making connection to the given desktop by wifi
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
import android.graphics.drawable.AnimationDrawable;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;

import com.soundroid.R;
import com.soundroid.net.DesktopData;
import com.soundroid.ui.UI;

/**
 * Activity for making connection to the given desktop
 */
public class WiFiConnectingActivity extends Activity {
	
	private static final String TAG = "WiFiConnectingActivity";                /**< The tag for writing to log */

	/**
	 * Creating the activity
	 * @param savedInstanceState
	 */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_connecting);
		
		ImageView imgView = (ImageView) findViewById(R.id.connImgView);
		imgView.setBackgroundResource(R.anim.animation);
		
		AnimationDrawable animDrawable = (AnimationDrawable) imgView.getBackground();
        if(animDrawable == null) {
            Log.e(TAG, "The AnimationDrawable instance of the image background is NULL");
            return;
        }
		animDrawable.start();
		
		final Intent intent = getIntent();
		if(intent == null) {
			Log.e(TAG, "onCreate(): The given intent is NULL");
			finish();
			return;
		}
		
		final DesktopData desktopData = (DesktopData)intent.getSerializableExtra(UI.DESKTOPS_DATA);
		if(desktopData == null) {
			Log.e(TAG, "onCreate(): The desktop data in the given intent is NULL");
			finish();
			return;
		}		
		
		if(UI.connector == null) {
			Log.e(TAG, "onCreate(): The network connector in the given intent is NULL");
			finish();
			return;
		}

		Thread thread = new Thread(getConnection(desktopData));
		thread.start();
	}

	/**
	 * Click on 'Back'
	 */
	//@Override
    public void onBackPressed() {
        setResult(RESULT_OK);
        finish();
    }

	/**
	 * Connect to desktop which data has given  
	 * @param data The data of the desktop for connecting to
	 * @return The runnable instance of the function
	 */
	private Runnable getConnection(final DesktopData data) {
		final String ip   = data.getAddress();
		final String port = data.getPort();
		
		return new Runnable() {
			
			@Override
			public void run() {
				
				if(!UI.connector.isAddrReachable(ip)) {
					setResult(RESULT_OK);
					finish();
					return;
				}
				
				try {
                    UI.connector.connect(ip, port);
				} catch (Exception e) {
					Log.e(TAG, "Can't connect to the host IP=" + ip + " by the PORT=" + port + ": " + e.toString());
					setResult(RESULT_OK);
					finish();
					return;
				}

                UI.connector.handShake();
				setResult(RESULT_OK);
				finish();
			}
		};
	}
}

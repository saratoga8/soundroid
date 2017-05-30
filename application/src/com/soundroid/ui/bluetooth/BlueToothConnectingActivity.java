/**
 * @file
 * Activity for making connection to the given desktop by bluetooth
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

package com.soundroid.ui.bluetooth;

import android.app.Activity;
import android.content.Intent;
import android.graphics.drawable.AnimationDrawable;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.widget.ImageView;

import com.soundroid.R;
import com.soundroid.ui.UI;

import java.io.IOException;
import java.util.List;


/**
 * The activity of connecting to a remote soundroid daemon by blue tooth
 */
public class BlueToothConnectingActivity extends Activity {
    private static final String TAG = "BTConnectingActivity";        /**< The tag for writing to log */

    public static final String CONNECTED_DEV_NAME = "Device_Name_For_Connecting";
    public static final String IS_REMOTE_DEV_FOUND = "Is_remote_dev_found";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_connecting);

        ImageView imgView = (ImageView) findViewById(R.id.connImgView);
        imgView.setBackgroundResource(R.anim.animation);

        AnimationDrawable animDrawable = (AnimationDrawable) imgView.getBackground();
        if(animDrawable == null) {
            Log.e(TAG, "The AnimationDrawable instance of the image background is NULL");
            finish();
        }
        animDrawable.start();

        final Intent intent = getIntent();
        final String connectedDevName = intent.getStringExtra(CONNECTED_DEV_NAME);

        Thread thread = new Thread(getConnection(connectedDevName));
        thread.start();
    }

    //    @Override
    public void onBackPressed() {
        setResult(RESULT_CANCELED);
        finish();
    }

    /**
     * Connect to desktop with the given name
     * @param connectedDevName The name of the desktop
     * @return The runnable instance of the function
     */
    private Runnable getConnection(final String connectedDevName) {
        return new Runnable() {

            @Override
            public void run() {
                if(UI.connector == null) {
                    Log.e(TAG, "The connector instance in UI is NULL");
                    finish();
                }

                final Intent intent = getIntentWithReachabilityOfDev(connectedDevName);
                setResult(RESULT_OK, intent);
                finish();
            }
        };
    }

    /**
     * Get the intent containing the result of connection to a remote desktop
     * @param devName The name of the desktop(device)
     * @return The intent instance
     */
    private Intent getIntentWithReachabilityOfDev(final String devName) {
        final Intent intent = new Intent();
        boolean result = false;
        try {
            UI.connector.connect(devName, "");
            result = UI.connector.handShake();
        } catch (IOException e) {
            Log.e(TAG, "Can't connect to the remote device " + devName);
        }
        intent.putExtra(IS_REMOTE_DEV_FOUND, result);
        return intent;
    }
}
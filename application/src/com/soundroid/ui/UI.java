/**
 * @file
 * The user interface abstract class
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

package com.soundroid.ui;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.Spinner;

import com.soundroid.R;
import com.soundroid.net.DesktopData;
import com.soundroid.net.NetConnector;
import com.soundroid.others.FileUtils;
import com.soundroid.ui.bluetooth.BluetoothUI;
import com.soundroid.ui.wifi.WifiUI;

import java.io.IOException;
import java.security.InvalidParameterException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

/**
 * User interface abstract class
 */
public abstract class UI implements AdapterView.OnItemSelectedListener {
    public static final String FILE_NAME = "file_name";

    private static String TAG = "UI";                                                       /**< The tag for logs */

    protected static String CONN_TYPE_CHECKED_RADIO_BUTT_ID_FILE_NAME = "conn_type";        /**< The name of the file containing the ID of
                                                                                                 the connection type radio button selected at last session */
    protected Context context;                                                              /**< The application's context */

    protected MuteButton muteButt;                                                           /**< The button for muting volume */

    protected Button buttEdit, buttNew;                                                      /**< The buttons Edit and New */

    protected RadioGroup connectionTypeRadio;                                                /**< The radio buttons group for connection types */

    protected Spinner desktopsNames;                                                         /**< The drop box of the desktops names for connecting */

    protected SeekBar volume;                                                                /**< The seek bar for changing volume in percents */

    /*! \var desktopsDatas
        \brief The txt_view_item of desktops data */
    protected List<DesktopData> desktopsDatas;

    private boolean connectionTypeChanged = false;                                           /**< The falg of connection type changing */

    public final static String DESKTOPS_DATA = "DESKTOPS_DATA";
    public final static int EDIT_DESKTOP_NAME   = 1,                                         /**< The selected action in the activity */
                            CREATE_DESKTOP_NAME = 2;

    public final static String SINGLE_DESKTOP_DATA = "DESKTOP_DATA";

    public static final int REQUEST_CODE_EDIT       = 1,                                     /**< The request code for a selected action */
                            REQUEST_CODE_CONNECTING = 2;

    public static final String IS_DEFAULT = "IS_DEFAULT";

    protected final int NO_EDIT = -1;
    protected int curEditedDesktopNameInd = NO_EDIT;                                          /**< The selected desktop name on the spinner at the time of clicking on the button 'Edit' */
    protected int curSelectedDesktopNameInd = 0;                                              /**< Currently selected desktop name on the spinner */

    protected boolean host_connection_checked = false;                                        /**< The connection to the currently selected desktop's host has checked */

    protected boolean redraw = false;                                                         /**< The activity should be redrawn */
    protected boolean redrawSoundVolUI = false;                                               /**< The part of UI for controlling sound volume should be redrawn */


    protected Activity activity;                                                              /**< The current activity instance */

    public static NetConnector connector;                                                     /**< The network connector instance */

    /**
     * Save the current UI data
     */
    public void saveUiData() {
        Log.d(TAG, "Saving ui data");

        saveCurConnectionType();
        saveDesktopsDatas();
    }

    /**
     * Get current network connector
     * @return The network connector instance
     */
    public NetConnector getConnector() {
        return connector;
    }

    /**
     * Save the txt_view_item of desktops' datas
     */
    protected void saveDesktopsDatas(String fileName) {
        Log.d(TAG, "Saving desktops data to file " + fileName);
        try {
            FileUtils.saveDataList(context, fileName, desktopsDatas);
        } catch (IOException e) {
            Log.e(TAG, "\tCan't save desktops datas to the file " + fileName + ": " + e.toString());
        }
    }

    /**
     * Save the datas of desktops
     */
    public abstract void saveDesktopsDatas();

    /**
     * Save current connection type
     */
    protected abstract void saveCurConnectionType();

    /**
     * Fill the list of desktops datas from file
     * @param fileName The file name with the data
     * @throws IOException
     */
    protected abstract void fillDesktopsListFromFile(final String fileName) throws IOException;

    /**
     * Initialization of the desktops data from file
     * @param fileName The file name with the data
     */
    protected void initDesktopsDatasFromFile(final String fileName) {
        Log.d(TAG, "Initialization of desktops names txt_view_item from the file " + fileName);

        if(desktopsDatas == null) {
            desktopsDatas = new LinkedList<DesktopData>();
        }
        try {
            desktopsDatas.clear();
            fillDesktopsListFromFile(fileName);
        }
        catch (IOException e) {
            Log.e(TAG, "Cant read data from the file " + fileName + ": " + e.toString());
        }
        catch (InvalidParameterException e) {
            Log.e(TAG, "Can't read data from the file " + fileName + ": " + e.toString());
        }
    }

    /**
     * Initialization of the desktops data from file
     */
    public abstract void initDesktopsDatasFromFile();

    /**
     * Initialization from file the radio button 'Connection type' checked at previous session
     */
    public static UI buildUiFromFile(final Activity activity) throws IOException, NumberFormatException {
        String str = "";
        try {
            str = FileUtils.getOneLineFromFile(activity.getApplicationContext(), CONN_TYPE_CHECKED_RADIO_BUTT_ID_FILE_NAME);
            final int num = Integer.parseInt(str);
            return (num == NetConnector.ConnectionTypes.BLUE_TOOTH.ordinal()) ? new BluetoothUI(activity): new WifiUI(activity);
        } catch (IOException e) {
            Log.e(TAG, "Can't read the previously used connection type from the file " + CONN_TYPE_CHECKED_RADIO_BUTT_ID_FILE_NAME + ": " + e.toString());
            return new WifiUI(activity);
        }
        catch (NumberFormatException e) {
            Log.e(TAG, "Can't convert the sting '" + str + "' to int: " + e.toString());
            throw e;
        }
    }

    /**
     * Build the UI with depends on the selected Radio button of connection type
     * @param activity The application's activity
     * @return The built UI
     */
    public UI buildUiBySelectedRadioButt(final Activity activity) {
        return (connectionTypeRadio.getCheckedRadioButtonId() == R.id.blueToothRadio) ? new BluetoothUI(activity): new WifiUI(activity);
    }

    /**
     * Set the flag of connection type changing
     * @param state The state of changing: true - connection type has changed
     */
    public synchronized void setConnectionTypeChanged(final boolean state) {
        connectionTypeChanged = state;
    }

    /**
     * Get the flag of connection type changing
     * @return true Connection type has
     */
    public synchronized boolean hasConnectionTypeChanged() {
        return connectionTypeChanged;
    }

    /**
     * Initialization of an user interface
     * @param connected There is connection to network
     */
    public void initUI(final boolean connected) {
        Log.d(TAG, "Init UI");

        initConnectionTypeRadioButt();
        initConnectionTypeRadioGroup();
        initDesktopNamesSpinner();
        initButtons();
        initVolumeUI();

        if(connected) {
            if( (desktopsDatas != null) && (!desktopsDatas.isEmpty()) ) {
                setVolumeUI();
            }
        }
    }

    /**
     * Initialize the radio button of connection type
     */
    protected abstract void initConnectionTypeRadioButt();

    /**
     * Initialization of the 'connection type' radio buttons
     */
    private void initConnectionTypeRadioGroup() {
        connectionTypeRadio = (RadioGroup) activity.findViewById(R.id.connTypeRadioGrp);
        connectionTypeRadio.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                setConnectionTypeChanged(true);
            }
        });
    }

    /**
     * Set the UI of sound volume
     */
    protected abstract  void setVolumeUI();

    /**
     * Show a dialog with the given text and the button 'OK'
     * @param txt The text for the dialog
     */
    protected void showOkDialog(String txt) {
        AlertDialog.Builder builder = new AlertDialog.Builder(activity);
        builder.setMessage(txt).setCancelable(false).setPositiveButton("OK", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int id) {  /** not used */ }
        });
        AlertDialog alert = builder.create();
        alert.show();
    }

    /**
     * Enable the seek bar for changing volume
     */
    protected void enableVolumeSeekBar() {
        String curVolPercent = connector.getCurVol();
        if(!TextUtils.isEmpty(curVolPercent)) {
            setSeekBar(Integer.parseInt(curVolPercent));
            volume.setEnabled(true);
        }
    }

    /**
     * Initialize the the UI for controlling volume percent
     */
    private void initVolumeUI() {
        initVolumeSeekBar();
//		equaliser.setVisibility(View.VISIBLE);
    }

    /**
     * Set values for the elements of the Volume UI(mute/unmute button, volume seekbar, etc)
     */
    protected void setVolumeUiElements() {
        muteButt.setEnabled(true);

        if(!connector.isMuted()) {
            enableVolumeSeekBar();
            muteButt.setShownMuted(false);
        }
        else {
            volume.setEnabled(false);
            muteButt.setShownMuted(true);
        }
    }

    /**
     * Initialize the seek bar for controlling volume percent
     */
    private void initVolumeSeekBar() {
        volume = (SeekBar) activity.findViewById(R.id.volumeSeekBar);
        volume.setOnSeekBarChangeListener(getOnSeekBarChangeListener ());
        volume.setEnabled(false);
    }

    /**
     * Get the listener of seek bar changes
     * @return
     */
    private SeekBar.OnSeekBarChangeListener getOnSeekBarChangeListener() {
        return new SeekBar.OnSeekBarChangeListener() {

            private int prevProgres;

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                final int progress = seekBar.getProgress();
                if(!connector.sendChgVol(progress - prevProgres))
                    seekBar.setProgress(prevProgres);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                prevProgres = seekBar.getProgress();
            }

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {  /** not used */  }
        };
    }

    /**
     * Initialization of the UI buttons
     */
    private void initButtons() {
        initEditButt();
        initNewButt();

        muteButt = new MuteButton();
    }

    /**
     * Initialize the 'New' button
     */
    protected abstract void initNewButt();

    /**
     * Initialize the 'Edit' button
     */
    protected abstract void initEditButt();

    /**
     * Initialization of the 'Desktops Names' spinner
     */
    public void initDesktopNamesSpinner() {
        Log.d(TAG, "Initialization of the desktops names spinner");

        desktopsNames = (Spinner) activity.findViewById(R.id.desktopsSpinner);
        desktopsNames.setOnItemSelectedListener(this);

        ArrayList<String> names = new ArrayList<String>();
        for(DesktopData data: desktopsDatas) {
            names.add(data.getName());
        }

        ArrayAdapter<String> spinnerArrAdapter = new ArrayAdapter<String>(context, android.R.layout.simple_spinner_item, names);
        spinnerArrAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        desktopsNames.setAdapter(spinnerArrAdapter);

        final int spinnerPos = (curEditedDesktopNameInd != NO_EDIT) ? curEditedDesktopNameInd: curSelectedDesktopNameInd;
        desktopsNames.setSelection(spinnerPos);
    }

    /**
     * Check whether the desktop with the given name is default.
     * Default means the data of the desktop with the given name is
     * at the head of the txt_view_item of desktops datas
     * @param name The name of a desktop
     * @return true The desktop is default
     */
    protected boolean isDesktopDataDefault(final String name) {
        if(TextUtils.isEmpty(name)) {
            Log.e(TAG, "The given desktop name is NULL or EMPTY");
            return false;
        }

        String defaultDesktopName = desktopsDatas.get(0).getName();
        return defaultDesktopName.equals(name);
    }

    /**
     * Get the desktop's data by the given name
     * @param name The name of the desktop
     * @return The desktop's data
     */
    protected DesktopData getDesktopDataByName(final String name) {
        if(TextUtils.isEmpty(name)) {
            throw new InvalidParameterException("The given name of a searched desktop data is NULL or EMPTY");
        }

        for(DesktopData data: desktopsDatas) {
            if(data.getName().equals(name)) {
                return data;
            }
        }
        throw new InvalidParameterException("There is no desktop data with the given name '" + name + "'");
    }

    /**
     * Remove the currently edited desktop data
     */
    protected void removeCurrentlyEditedDesktopData() {
        Log.d(TAG, "Remove the data of the desktop at the index " + curEditedDesktopNameInd + " from the txt_view_item of desktops");

        if(curEditedDesktopNameInd == NO_EDIT) {
            Log.e(TAG, "There is no selection within the txt_view_item");
            return;
        }

        desktopsDatas.remove(curEditedDesktopNameInd);
        curEditedDesktopNameInd = NO_EDIT;
        curSelectedDesktopNameInd = 0;

//		refreshDesktopsDatasList = true;
    }

    /**
     * Set the seek bar's progress by the given percent
     * @param percent The percent of progress
     */
    private void setSeekBar(final int percent) {
        volume.setMax(100);
        volume.setProgress(percent);
    }

    /**
     * Process the result of the activity
     * @param requestCode The request code
     * @param data The given intent with data
     */
    public abstract void processActivityResult(final int requestCode, final Intent data);

    /**
     * Set the flag for redrawing UI
     * @param flag The flag
     */
    public synchronized void redraw(final boolean flag) {
        redraw = flag;
    }

    /**
     * Should the UI be redrawn
     * @return true The UI should be redrawn
     */
    public synchronized boolean shouldBeRedrawn() {
        return redraw;
    }

    /**
     * Set the flag for redrawing of the UI of the sound volume
     * @param flag The flag
     */
    public synchronized void redrawSoundVolUI(final boolean flag) {
        redrawSoundVolUI = flag;
    }

    /**
     * Should the UI of the sound volume be redrawn
     * @return true The UI should be redrawn
     */
    public synchronized boolean shouldBeRedrawnSoundVolUI() { return redrawSoundVolUI; }

    /**
     * Clear the list of desktops data
     */
    public void clrDesktopsData() {
        if (desktopsDatas != null) {
            desktopsDatas.clear();
        }
    }

    /**
     * Connect to a network by opening dialog for user selection
     */
    public abstract void connectToNetByDialog();

    /**
     * Class of the button for changing volume states: mute, unmute
     */
    protected class MuteButton {

        private Button butt;                         /**< The button instance */
        private boolean shownMuted = false;          /**< The button shown as muted */

        /**
         * Constructor for initializing the button and its state
         */
        public MuteButton() {
            butt = (Button) activity.findViewById(R.id.muteButt);
            init();
        }

        /**
         * Set the button enabled or disabled
         * @param enabled To enable the button
         */
        public void setEnabled(final boolean enabled) {
            butt.setEnabled(enabled);
        }

        /**
         * Set the button's background image depending on the given state
         * @param muted The state of the button
         */
        public void setShownMuted(final boolean muted) {
            if(muted != shownMuted) {
                final int picID = (muted) ? R.drawable.muted: R.drawable.unmuted;
                shownMuted = muted;
                butt.setBackgroundResource(picID);

                if(!shownMuted) {
                    enableVolumeSeekBar();
                }
                else {
                    volume.setEnabled(false);
                }
            }
        }

        /**
         * Initialize the button
         */
        private void init() {
            butt.setEnabled(false);
            butt.setOnClickListener(getMuteButtOnClickListener ());
        }

        private View.OnClickListener getMuteButtOnClickListener() {
            return new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if(connector != null) {
                        if(connector.sendMuteState(!shownMuted)) {
                            setShownMuted(!shownMuted);
                            butt.setEnabled(true);
                        }
                    }
                    else {
                        Log.e(TAG, "Click on the button. But net connector is NULL");
                    }
                }
            };
        }
    }

    /**
     * Equalizer for the volume's seek bar
     */
    public static class EqualiserView extends View {
        private final int RECTS_NUM = 42;        /**< The number of rectangles in the equalizer */

        private RectF rects[] = null;            /**< Rectangles array */

        private Paint paint;                     /**< The paint instance */

        /**
         * Constructor
         * @param context The context instance for super
         */
        public EqualiserView(Context context) {
            super(context);
            paint = new Paint();
        }

        public EqualiserView(Context context, AttributeSet attrs) {
            super(context, attrs);
            paint = new Paint();
        }

        public EqualiserView(Context context, AttributeSet attrs, int defStyle) {
            super(context, attrs, defStyle);
        }

        /**
         * Initializing of the equalizer's rectangles
         * @return The rectangles array
         */
        private RectF[] initRectangles() {
            RectF [] rectangles = new RectF[RECTS_NUM];

            final float WIDTH = getMeasuredWidth();
            final float STEP_WIDTH = WIDTH / RECTS_NUM;
            final float HEIGHT = getMeasuredHeight();

            final float K = HEIGHT / (2 * WIDTH);

            for(int i = 0; i < RECTS_NUM; ++i) {
                float leftx = i * STEP_WIDTH;
                float rightx = leftx + STEP_WIDTH;
                float topy = HEIGHT - (K * rightx);

                rectangles[i] = new RectF(leftx, topy, rightx, HEIGHT);
            }

            return rectangles;
        }

        @Override
        /**
         * Initializing the rectangles and coloring them according to position
         */
        public void onDraw(Canvas canvas) {
            super.onDraw(canvas);

            if(rects == null)
                rects = initRectangles();

            final int COLOR_AREAS_NUM           = 3;
            final int RECTS_NUM_IN_COLOR_AREA   = RECTS_NUM / COLOR_AREAS_NUM;
            final int COLOR_CHG_STEP            = 255 / RECTS_NUM_IN_COLOR_AREA;
            final int YELLOW_AREA_LAST_RECT_NUM = RECTS_NUM_IN_COLOR_AREA;
            final int GREEN_AREA_LAST_RECT_NUM  = RECTS_NUM_IN_COLOR_AREA * 2;

            int red   = 255;
            int green = 255;
            int blue  = 255;

            int i = 0;
            while(i < YELLOW_AREA_LAST_RECT_NUM) {
                RectF curRect = rects[i++];
                blue -= COLOR_CHG_STEP;
                paint.setColor(Color.rgb(red, green, blue));
                canvas.drawRoundRect(curRect, curRect.left, curRect.bottom, paint);
            }
            while(i < GREEN_AREA_LAST_RECT_NUM) {
                RectF curRect = rects[i++];
                red -= COLOR_CHG_STEP;
                paint.setColor(Color.rgb(red, green, blue));
                canvas.drawRoundRect(curRect, curRect.left, curRect.bottom, paint);
            }
            while(i < RECTS_NUM) {
                RectF curRect = rects[i++];
                red += COLOR_CHG_STEP;
                green -= COLOR_CHG_STEP;
                paint.setColor(Color.rgb(red, green, blue));
                canvas.drawRoundRect(curRect, curRect.left, curRect.bottom, paint);
            }
        }
    }

    /**
     * Re-initialize connection to a network
     */
    protected abstract void reInitConnector();

    /**
     * If an item of the desktops names drop box selected
     * the volume percent of the selected desktop should be set
     */
    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
        if(curSelectedDesktopNameInd != pos) {
            curSelectedDesktopNameInd = pos;

            reInitConnector();

            host_connection_checked = false;
            setVolumeUI();
        }
    }

    /**
     * Not used
     */
    @Override
    public void onNothingSelected(AdapterView<?> arg0) { /** not used */ }
}

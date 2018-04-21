/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import android.app.Activity;
import org.cocos2dx.lib.*;

import android.os.Bundle;
import android.os.Environment;

import org.cocos2dx.lib.Cocos2dxActivity;

// Generic usage:
import java.util.*; 
import android.util.Log;
import android.os.Build;
import android.provider.Settings.Secure;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.DataInputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import org.json.JSONObject;
import org.json.JSONException;
import android.os.RemoteException;
import java.util.ArrayList;

// IAP:
import com.android.vending.billing.IInAppBillingService;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.os.IBinder;

// Sharing via varied platforms:
import android.content.Intent;

// Local Notification / Alarm:
// import java.util.Calendar;
// import android.app.NotificationManager;
// import android.app.PendingIntent;
import android.content.Context;
// import android.app.AlarmManager;

// this Project:
import com.spark.atom2048.R;

// Crashlytics packages:
//import io.fabric.sdk.android.Fabric;
//import com.crashlytics.android.Crashlytics;
//import com.crashlytics.android.ndk.CrashlyticsNdk;

// Multidex:
//import android.support.multidex.MultiDex;

public class AppActivity extends Cocos2dxActivity {
	private Cocos2dxGLSurfaceView glSurfaceView;
	private static Activity sActivity;
	
	/* In-app consumption starts : */
	private static IInAppBillingService mService;
	
	ServiceConnection mServiceConn = new ServiceConnection() {
	   @Override
	   public void onServiceDisconnected(ComponentName name) {
		   mService = null;
	   }

	   @Override
	   public void onServiceConnected(ComponentName name,
		  IBinder service) {
		   mService = IInAppBillingService.Stub.asInterface(service);
	   }
	};
	
	/* In-app consumption ends : */
	
	@Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
		sActivity = this;

		// starting Fabric Crashlytics:
		//Fabric.with(this, new Crashlytics(), new CrashlyticsNdk()); //Crashlytics.start(this);
		
		/* In-app consumption starts : */
		// Setting for InApp Billing:
		Intent serviceIntent = new Intent("com.android.vending.billing.InAppBillingService.BIND");
		serviceIntent.setPackage("com.android.vending");
		bindService(serviceIntent, mServiceConn, Context.BIND_AUTO_CREATE);
		/* In-app consumption ends : */
    }
	
	@Override
	public void onDestroy() {
		super.onDestroy();
		if (mService != null) {
			unbindService(mServiceConn);
		}
	}
	
	// @Override
	// protected void attachBaseContext(Context base) {
		 // super.attachBaseContext(base);
		 // MultiDex.install(this);
	// }

    public Cocos2dxGLSurfaceView onCreateView()
    {
        glSurfaceView = new Cocos2dxGLSurfaceView(this);

        this.hideSystemUI();

        // create stencil buffer
        glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);

        return glSurfaceView;
    }

    public void onWindowFocusChanged(boolean hasFocus)
    {	
		Log.i("calling the onWindowFocusChanged in java code dear: ", "kuch nhi");
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus)
        {
            this.hideSystemUI();
        }
    }

    private void hideSystemUI()
    {
        // Set the IMMERSIVE flag.
        // Set the content to appear under the system bars so that the content
        // doesn't resize when the system bars hide and show.
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
        glSurfaceView.setSystemUiVisibility(
                Cocos2dxGLSurfaceView.SYSTEM_UI_FLAG_LAYOUT_STABLE 
                | Cocos2dxGLSurfaceView.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | Cocos2dxGLSurfaceView.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | Cocos2dxGLSurfaceView.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
                | Cocos2dxGLSurfaceView.SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
                | Cocos2dxGLSurfaceView.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
		} else { // For Android 4.4 or less:
        glSurfaceView.setSystemUiVisibility(
                Cocos2dxGLSurfaceView.SYSTEM_UI_FLAG_LAYOUT_STABLE 
                | Cocos2dxGLSurfaceView.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | Cocos2dxGLSurfaceView.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | Cocos2dxGLSurfaceView.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
                | Cocos2dxGLSurfaceView.SYSTEM_UI_FLAG_FULLSCREEN); // hide status bar			
		}
    }

    static
    {
        System.loadLibrary("cocos2dcpp");
    }  
	
    public static String getUUID()
    {	
		String android_id = "", savedUuid = "";
		// Don't use randomUUID() since it's randomely generated and once the app is uninstalled and re-installed,
		// the id should remain same but from this function it may not remain same.
		// A class that represents an immutable universally unique identifier (UUID). A UUID represents a 128-bit value.
		// UUID randomUUID () : Static factory to retrieve a type 4 (pseudo randomly generated) UUID. The UUID is 
		// generated using a cryptographically strong pseudo random number generator.
		// checking is uuid already exists:
		if(isExternalStorageReadable()){
			// Is file exists:
			try {
				// get the path to sdcard
				File sdcard = Environment.getExternalStorageDirectory();
				File dir = new File(sdcard.getAbsolutePath() + "/atom2048/");
				File atom2048file = new File(dir, "atom.txt");

				FileInputStream fis = new FileInputStream(atom2048file);
				DataInputStream in = new DataInputStream(fis);
				BufferedReader br = new BufferedReader(new InputStreamReader(in));
				String strLine;
				while ((strLine = br.readLine()) != null) {
					savedUuid = savedUuid + strLine;
				}
				in.close();
			} catch (IOException e) {
				Log.v("getUUID", "Exception in uuid retrieving.");
				e.printStackTrace();
			}
			
			// If file does not exist or file is empty or invalid number of characters:
			// Is file content no empty && character length is == 36:
			if(savedUuid.length() == 36){
				Log.v("getUUID", "Saved uuid retrieved:" + savedUuid);
				// return the newly creatd uuid:
				return savedUuid;
			}
		}
		
		// Otherwise create a new uuid, save it and return it:
		// Create new uuid, 
		android_id = UUID.randomUUID().toString();
		
		// Save it in external storage,
		if(isExternalStorageWritable()){
			try{
				// get the path to sdcard
				File sdcard = Environment.getExternalStorageDirectory();
				// to this path add a new directory path
				File dir = new File(sdcard.getAbsolutePath() + "/atom2048/");
				
				// create this directory if not already created
				dir.mkdir();
				// create the file in which we will write the contents
				File atom2048file = new File(dir, "atom.txt");
				FileOutputStream outStream = new FileOutputStream(atom2048file);
				outStream.write(android_id.getBytes());
				outStream.close();
				Log.v("getUUID", "UUID saved is external storage: " + android_id);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		Log.e("getUUID:",android_id);						
        return android_id;
    }

	public static String getDeviceName() {
		String deviceName;
		if (Build.MODEL.startsWith("KF")){
			deviceName = "KINDLE";
		} else {
			deviceName = "ANDROID";
		}
		
		return deviceName;
	}

	public static String getAppName() {		
		return "The Tile";
	}

	private String capitalize(String s) {
		if (s == null || s.length() == 0) {
			return "";
		}
		char first = s.charAt(0);
		if (Character.isUpperCase(first)) {
			return s;
		} else {
			return Character.toUpperCase(first) + s.substring(1);
		}
	}
	
	public static void consumeIAPProducts() throws JSONException, Exception, RemoteException{
		Log.v( "consumeIAP", "Initialized.");

		if(getDeviceName().equals(DeviceType.Kindle)){ // Kindle
			// Nothing for now.
		} else { // Google Play Store
			// Note the null is the continueToken you may not get all of the purchased items
			// in one call - check ownedItems.getString("INAPP_CONTINUATION_TOKEN") for 
			// the next continueToken and re-call with that until you don't get a token
			Bundle ownedItems = mService.getPurchases(3, getContext().getPackageName(), "inapp", null);
			// Check response
			int responseCode = ownedItems.getInt("RESPONSE_CODE");
			if (responseCode != 0) {
			   throw new Exception("Error");
			}
			// Get the list of purchased items
			ArrayList<String> purchaseDataList = 
				ownedItems.getStringArrayList("INAPP_PURCHASE_DATA_LIST");
			for (String purchaseData : purchaseDataList) {
				JSONObject jsonObj = new JSONObject(purchaseData);
				String purchaseToken = jsonObj.optString("token", jsonObj.optString("purchaseToken"));
				Log.v( "consumeIAP", "purchaseDataList:" + purchaseData);
				// Consume purchaseToken, handling any errors
				int responseConsume = mService.consumePurchase(3, getContext().getPackageName(), purchaseToken);
				if(responseConsume == 0){
					Log.v( "consumeIAP", "Item consumption success. purchaseDataList:" + purchaseData);				
				} else {
					Log.v( "consumeIAP", "Item consumption Error. purchaseDataList:" + purchaseData);								
				}
			}
		}
	}
	
	// Share button:
	public static void callShare(){
		Log.v("AppActivity", "callShare initiated");

		Intent sharingIntent = new Intent(Intent.ACTION_SEND);
		sharingIntent.setType("text/plain");
		sharingIntent.putExtra(android.content.Intent.EXTRA_TEXT, "Check out \"" + getAppName() + "\"\n\nhttps://play.google.com/store/apps/details?id=com.spark.atom2048\n\nIt's an amazing game and you won't be able to take your mind off it. Try out now and do not forget to earn rewards by recommending it to you friends.\nEnjoy!");
		sActivity.startActivity(Intent.createChooser(sharingIntent, "Share " + getAppName() + " using"));
	}
	
	/* Checks if external storage is available for read and write */
	public static boolean isExternalStorageWritable() {
		Log.v("isExternalStorageReadable", "External storage writable check");
		String state = Environment.getExternalStorageState();
		if (Environment.MEDIA_MOUNTED.equals(state)) {
			Log.v("isExternalStorageReadable", "External storage is writable");
			return true;
		}
		return false;
	}

	/* Checks if external storage is available to at least read */
	public static boolean isExternalStorageReadable() {
		Log.v("isExternalStorageReadable", "External storage readable check");

		String state = Environment.getExternalStorageState();
		if (Environment.MEDIA_MOUNTED.equals(state) ||
			Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
			Log.v("isExternalStorageReadable", "External storage is readable");
			return true;
		}
		return false;
	}
}

class DeviceType {
	public final static String Kindle = "KINDLE";
	public final static String Android = "ANDROID";
}

class ModelType {
	public final static String Kindle = "KF";
}
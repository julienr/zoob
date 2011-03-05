package net.fhtagn.zoobgame.menus;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.FrameLayout.LayoutParams;
import android.widget.Toast;
import net.fhtagn.zoob_demo.R;
import net.fhtagn.zoobgame.Zoob;

public class MultiplayerMenuView extends MenuView {
	private final static String TAG = "MultiplayerMenuView";
	private final Zoob activity;
	
	private final EditText serverIP;
	private final Button connectButton;
	
	public MultiplayerMenuView(final Zoob zoob) {
		super(zoob, R.id.container);
		
		this.activity = zoob;
		LayoutInflater inflater = (LayoutInflater)activity.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	  this.addView(inflater.inflate(R.layout.multimenu, null), 
	  		new FrameLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
	  
	  setBackground();
	  
	  serverIP = (EditText)findViewById(R.id.server_ip);
	  connectButton = (Button)findViewById(R.id.btn_connect);
	  connectButton.setOnClickListener(new OnClickListener () {
			@Override
      public void onClick(View v) {
				final String ip = serverIP.getText().toString();
				zoob.playClient(ip);
	      Toast.makeText(activity, "Connect to : " + ip, Toast.LENGTH_LONG).show();
      }
	  });
	}

}

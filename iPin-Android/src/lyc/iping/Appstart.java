package lyc.iping;



import java.io.File;

import android.os.Bundle;
import android.os.Handler;
import android.app.Activity;
import android.content.Intent;

public class Appstart extends Activity{

	@Override
	public void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);	
		setContentView(R.layout.appstart);
		String AppPath = getApplicationContext().getFilesDir().getAbsolutePath() + "/";
		System.out.print("AppPath:" + AppPath);
		File file = new File(AppPath + "HeadImage");
		if(!file.exists()) file.mkdir();		
		
	new Handler().postDelayed(new Runnable(){
		@Override
		public void run(){
			Intent intent = new Intent (Appstart.this,LoginActivity.class);			
			startActivity(intent);			
			Appstart.this.finish();
		}
	}, 2000);
   }
}
package lyc.iping;

import java.io.File;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.LevelListDrawable;
import android.os.Bundle;
import android.text.Editable;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

public class ReleasedActivity extends Activity implements OnClickListener{
    /** Called when the activity is first created. */

	private Button mBtnSend;
	private Button mBtnBack;
	private String info_detail = null;
	private String info_from = null;
	private String info_to = null;
	private String info_date = null;
	private String info_username = null;
	
	private TextView current_detail = null;
	private TextView current_from = null;
	private TextView current_to = null;
	private TextView current_date = null;
	private TextView current_username = null;
	private ImageView current_HeadImage = null;
	private int position;
	
	
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.released);
        //启动activity时不自动弹出软键盘
        getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN); 
        initView();
    }
    
    
    public void initView()
    {
    	mBtnSend = (Button) findViewById(R.id.btn_add);
    	mBtnSend.setOnClickListener(this);
    	mBtnBack = (Button) findViewById(R.id.btn_back);
    	mBtnBack.setOnClickListener(this);
    	
    	current_detail = (TextView)findViewById(R.id.tv_Detail);
    	current_from = (TextView)findViewById(R.id.tv_From);
    	current_to = (TextView)findViewById(R.id.tv_To);
    	current_date = (TextView)findViewById(R.id.tv_Date);
    	current_username = (TextView)findViewById(R.id.released_username);
    	current_HeadImage = (ImageView)findViewById(R.id.released_HeadImage);
    	
    	position=getIntent().getExtras().getInt("position")+1;
    	DatabaseHelper dbHelper = new DatabaseHelper(ReleasedActivity.this,"iPin");
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		Cursor cursor = db.query("info", new String[]{"info_ID","info_HeadImageVersion","info_username","info_from","info_to","info_date","info_detail","info_time"}, null, null, null, null, null);
		if(cursor.move(position))
		{			
			info_detail = cursor.getString(cursor.getColumnIndex("info_detail"));
			current_detail.setText(info_detail);
			info_from = cursor.getString(cursor.getColumnIndex("info_from"));
			current_from.setText(info_from);
			info_to = cursor.getString(cursor.getColumnIndex("info_to"));
			current_to.setText(info_to);
			info_date = cursor.getString(cursor.getColumnIndex("info_date"));
			current_date.setText(info_date);
			info_username = cursor.getString(cursor.getColumnIndex("info_username"));
			current_username.setText(info_username);
			String ID=cursor.getString(cursor.getColumnIndex("info_ID"));
			int HeadImageVersion=cursor.getInt(cursor.getColumnIndex("info_HeadImageVersion"));
			
			String AppPath = getApplicationContext().getFilesDir().getAbsolutePath() + "/";
	        String ImgPath = AppPath+"HeadImage/"+ID+"_"+HeadImageVersion+".jpg";
	        File ImgFile = new File(ImgPath);
	        if(ImgFile.exists())
	        {
	        	Bitmap HeadImage = null;
	        	HeadImage = BitmapFactory.decodeFile(ImgPath,null);
	        	current_HeadImage.setImageBitmap(HeadImage);
	        }
	        else
	        	current_HeadImage.setImageResource(R.drawable.default_head);
		}
		db.close();
		dbHelper.close();
    }
    


	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch(v.getId())
		{
		case R.id.btn_add:
			break;
		case R.id.btn_back:
			finish();
			break;
		}
	}
	

}
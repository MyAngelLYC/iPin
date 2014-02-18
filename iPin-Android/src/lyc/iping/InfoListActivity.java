package lyc.iping;


import java.io.InputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.security.MessageDigest;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;

import lyc.iping.LoginActivity.UserLoginTask;

import org.w3c.dom.Text;

import android.app.Activity;
import android.app.ListActivity;
import android.content.ContentValues;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.drawable.LevelListDrawable;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.Editable;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;


public class InfoListActivity extends ListActivity{
    /** Called when the activity is first created. */
		
	//ArrayList<HashMap<String, String>> list = new ArrayList<HashMap<String, String>>();
	
	String info_username = null;
	String info_from = null;
	String info_to = null;
	String info_date = null;
	String info_detail = null;
	
	private ImageView img_setting = null;
	private List<InfoListMsgEntity> mDataArrays = null;
	private InfoListMsgViewAdapter mAdapter = null;
	private InfoRefreshTask mInfoRefreshTask = null;
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.info_list_view);
//        启动activity时不自动弹出软键盘
        getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN); 
        initView();
    }
    
    public void initView()
    {
    	/*
    	DatabaseHelper dbHelper = new DatabaseHelper(InfoListActivity.this,"info");
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		Cursor cursor = db.query("info", new String[]{"info_username","info_from","info_to","info_date","info_detail"}, null, null, null, null, null);
		while(cursor.moveToNext())
		{
			info_from = cursor.getString(cursor.getColumnIndex("info_from"));
			info_to = cursor.getString(cursor.getColumnIndex("info_to"));
			info_date = cursor.getString(cursor.getColumnIndex("info_date"));
			info_username = cursor.getString(cursor.getColumnIndex("info_username"));
			
            HashMap<String, String> map = new HashMap<String, String>();
            map.put("info_username", info_username);
            map.put("info_date", info_date);
            map.put("info_from", info_from);
            map.put("info_to", info_to);
            list.add(map);
		}
		db.close();
		dbHelper.close();
		InfoListMsgViewAdapter listAdapter = new InfoListMsgViewAdapter(this, list,
				R.layout.info_item_msg_text, new String[] { "info_username", "info_date", "info_from", "info_to"},
				new int[] { R.id.info_username,R.id.info_date, R.id.info_from,R.id.info_to});
		setListAdapter(listAdapter);
		*/
		img_setting = (ImageView)findViewById(R.id.img_settings);
		img_setting.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				Intent intent = new Intent (InfoListActivity.this,SettingActivity.class);
				startActivity(intent);
				InfoListActivity.this.finish();
			}			
		});
		mDataArrays = new ArrayList<InfoListMsgEntity>();
		/*
		InfoListMsgEntity entity = new InfoListMsgEntity();
		entity.setID("000000001");
		entity.setHeadImageVersion("1");
		entity.setUsername("user");
		entity.setFrom("aaa");
		entity.setTo("bbb");
		entity.setDate("7/7");
		mDataArrays.add(entity);*/
		mAdapter = new InfoListMsgViewAdapter(this, mDataArrays);
		setListAdapter(mAdapter);
		mInfoRefreshTask = new InfoRefreshTask();
		mInfoRefreshTask.execute((Void) null); 
    }
    
    @Override  
    protected void onActivityResult(int requestCode, int resultCode, Intent data)  
    {  
        //可以根据多个请求代码来作相应的操作   
        if(20==resultCode)  
        { /* 
            String cnfrom=data.getExtras().getString("cnfrom");  
            String cnto=data.getExtras().getString("cnto");  
            String cndate=data.getExtras().getString("cndate");  
            String cnname=data.getExtras().getString("cnname");  
            
            HashMap<String, String> map = new HashMap<String, String>();
            map.put("info_username", cnname);
            map.put("info_date", cndate);
            map.put("info_from", cnfrom);
            map.put("info_to", cnto);
            list.add(map);
    		InfoListMsgViewAdapter listAdapter = new InfoListMsgViewAdapter(this, list,
    				R.layout.info_item_msg_text, new String[] { "info_username", "info_date", "info_from", "info_to"},
    				new int[] { R.id.info_username,R.id.info_date, R.id.info_from,R.id.info_to});
    		setListAdapter(listAdapter);*/
        }
        else if(50==resultCode)
        {
        	mInfoRefreshTask = new InfoRefreshTask();
    		mInfoRefreshTask.execute((Void) null); 
        }
        super.onActivityResult(requestCode, resultCode, data);  
    }  

	//设置标题栏右侧按钮的作用
	public void btnmainright(View v) {  
		Intent intent = new Intent (InfoListActivity.this,EditActivity.class);				
		startActivityForResult(intent,100);
		//finish();
	}  	
	
	@Override
	protected void onListItemClick(ListView l, View v, int position, long id) {
		// TODO Auto-generated method stub
		super.onListItemClick(l, v, position, id);
		Intent intent = new Intent (InfoListActivity.this,ReleasedActivity.class);
		intent.putExtra("position", position);
		startActivity(intent);	
	}
	
	public class InfoRefreshTask extends AsyncTask<Void, Void, Boolean> {
		private Socket socket = null;		
        PrintWriter out =  null;
        
		@Override
		protected Boolean doInBackground(Void... params) {
			// TODO: attempt authentication against a network service.
			try {
				socket = new Socket(getString(R.string.Server_IP),Integer.parseInt(getString(R.string.Server_Port)));
		        out = new PrintWriter(socket.getOutputStream(),true);	
		        out.print("InfoListRefresh");
		        out.flush();
		        mDataArrays.clear();
		        InputStream br = socket.getInputStream();
				byte[] buffer =new byte[2048];
				int readSize=br.read(buffer);				
				if(readSize>0)
				{
					String InfoRefreshMsg=new String(buffer,0,readSize);
					if(InfoRefreshMsg.contains("InfoRefreshNone")) return false;
					String[] temp = InfoRefreshMsg.split(" ");
					
					HeadImageDownloader downloader = new HeadImageDownloader();
					
					DatabaseHelper dbHelper = new DatabaseHelper(InfoListActivity.this,"iPin");
					SQLiteDatabase db = dbHelper.getWritableDatabase();
					db.delete("info", null, null);
					for(int i=0;i<Integer.parseInt(temp[0]);i++)
					{
						InfoListMsgEntity entity = new InfoListMsgEntity();
						String ID,HeadImageVersion,username,from,to,date,detail,time;
						ID=temp[i*8+1];
						HeadImageVersion=temp[i*8+2];
						username=temp[i*8+3];
						from=temp[i*8+4];
						to=temp[i*8+5];
						date=temp[i*8+6];
						detail=temp[i*8+7];
						time=temp[i*8+8];
						entity.setID(ID);
						entity.setHeadImageVersion(HeadImageVersion);
						entity.setUsername(username);
						entity.setFrom(from);
						entity.setTo(to);
						entity.setDate(date);
						mDataArrays.add(entity);
						ContentValues values = new ContentValues();
						values.put("info_ID", ID);
						values.put("info_HeadImageVersion", HeadImageVersion);
						values.put("info_username", username);
						values.put("info_from", from);
						values.put("info_to", to);						
						values.put("info_date", date);
						values.put("info_detail", detail);
						values.put("info_time", time);
						db.insert("info", null, values);
						downloader.download(InfoListActivity.this, ID, HeadImageVersion);						
					}
					db.close();
					socket.close();
					return true;
				}				
			} catch (Exception e) {
				return false;
			}
			
			// TODO: register the new account here.
			return false;
		}

		@Override
		protected void onPostExecute(final Boolean success) {
			mInfoRefreshTask = null;			
			if (success) {
				mAdapter.notifyDataSetChanged();
			} else {
				
			}			
			
		}

		@Override
		protected void onCancelled() {
			mInfoRefreshTask = null;
		}		
	}
	
}
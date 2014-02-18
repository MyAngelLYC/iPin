package lyc.iping;

import android.app.Activity;
import android.content.ContentValues;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity{
	private TextView textView = null;
	private Button LogoutButton = null;
	String username = null;
	String password = null;
	String sex = null;
	String autoLogin = null;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		textView = (TextView) findViewById(R.id.LoginTextView);
		LogoutButton = (Button) findViewById(R.id.LogoutButton);
		DatabaseHelper dbHelper = new DatabaseHelper(MainActivity.this,"iPin");
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		Cursor cursor = db.query("user", new String[]{"username","password","sex","autoLogin"}, null, null, null, null, null);
		while(cursor.moveToNext())
		{
			username = cursor.getString(cursor.getColumnIndex("username"));
			password = cursor.getString(cursor.getColumnIndex("password"));
			sex = cursor.getString(cursor.getColumnIndex("sex"));
			autoLogin = cursor.getString(cursor.getColumnIndex("autoLogin"));
			textView.setText(username + " " + password + " " + sex + " " + autoLogin + " welcome!");
		}
		db.close();
		dbHelper.close();
		LogoutButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
				DatabaseHelper dbHelper = new DatabaseHelper(MainActivity.this,"iPin");
				SQLiteDatabase db = dbHelper.getWritableDatabase();
				ContentValues value = new ContentValues();
				value.put("autoLogin", false);
				db.update("user",value,"username=?",new String[]{username});
				db.close();
				dbHelper.close();
				Intent intent = new Intent (MainActivity.this,LoginActivity.class);			
				startActivity(intent);			
				MainActivity.this.finish();
			}
		});
	}	
}

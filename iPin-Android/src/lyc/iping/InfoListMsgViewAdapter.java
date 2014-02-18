package lyc.iping;

import java.io.File;
import java.util.List;
import java.util.Map;

import lyc.iping.EditMsgViewAdapter.ViewHolder;
import lyc.iping.EditMsgViewAdapter.ViewHolderCom;

import android.R.color;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.SimpleAdapter;
import android.widget.TextView;

public class InfoListMsgViewAdapter extends BaseAdapter {
	private LayoutInflater mInflater = null;	
	private List<InfoListMsgEntity> coll;
    private Context ctx;
    
	
	public View getView(int position, View convertView, ViewGroup parent) {
		InfoListMsgEntity entity = coll.get(position);
    		
    	ViewHolder viewHolder = null;	
	    if (convertView == null)
	    {
	    	convertView = mInflater.inflate(R.layout.info_item_msg_text, null);
		    viewHolder = new ViewHolder();
		    viewHolder.head = (ImageView) convertView.findViewById(R.id.head);
		    viewHolder.info_username = (TextView) convertView.findViewById(R.id.info_username);
			viewHolder.info_from = (TextView) convertView.findViewById(R.id.info_from);
			viewHolder.info_to = (TextView) convertView.findViewById(R.id.info_to);
			viewHolder.info_date = (TextView) convertView.findViewById(R.id.info_date);				  
			convertView.setTag(viewHolder);				  
			
			viewHolder.info_username.setText(entity.getUsername());
			viewHolder.info_from.setText(entity.getFrom());
			viewHolder.info_to.setText(entity.getTo());
			viewHolder.info_date.setText(entity.getDate());
			String AppPath = ctx.getApplicationContext().getFilesDir().getAbsolutePath() + "/";
	        String ImgPath = AppPath+"HeadImage/"+entity.getID()+"_"+entity.getHeadImageVersion()+".jpg";
	        File ImgFile = new File(ImgPath);
	        if(ImgFile.exists())
	        {
	        	Bitmap HeadImage = null;
	        	HeadImage = BitmapFactory.decodeFile(ImgPath,null);
	        	viewHolder.head.setImageBitmap(HeadImage);
	        }
	        else
	        	viewHolder.head.setImageResource(R.drawable.default_head);
	    }else{
	          viewHolder = (ViewHolder) convertView.getTag();
	          String AppPath = ctx.getApplicationContext().getFilesDir().getAbsolutePath() + "/";
		       String ImgPath = AppPath+"HeadImage/"+entity.getID()+"_"+entity.getHeadImageVersion()+".jpg";
	          File ImgFile = new File(ImgPath);
	          if(ImgFile.exists())
	          {
	        	  Bitmap HeadImage = null;
	        	  HeadImage = BitmapFactory.decodeFile(ImgPath,null);
	        	  viewHolder.head.setImageBitmap(HeadImage);
	          }
	          else
	        	  viewHolder.head.setImageResource(R.drawable.default_head);
	          
	          viewHolder.info_username.setText(entity.getUsername());
			  viewHolder.info_from.setText(entity.getFrom());
			  viewHolder.info_to.setText(entity.getTo());
			  viewHolder.info_date.setText(entity.getDate());
	    }
		return convertView;
	}	
	
	public InfoListMsgViewAdapter(Context context, List<InfoListMsgEntity> coll) {
        ctx = context;
        this.coll = coll;
        mInflater = LayoutInflater.from(context);
    }

	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return coll.size();
	}

	@Override
	public Object getItem(int position) {
		// TODO Auto-generated method stub
		return coll.get(position);
	}

	@Override
	public long getItemId(int position) {
		// TODO Auto-generated method stub
		return position;
	}
	
	static class ViewHolder {
    	public ImageView head;
        public TextView info_username;
        public TextView info_from;
        public TextView info_to;
        public TextView info_date;
    }

}

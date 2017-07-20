package com.mac2corvallis.mac2uconcepts;

import android.content.Context;
import android.support.v4.view.PagerAdapter;
import android.view.LayoutInflater;
import android.view.View;

/**
 * Created by THE HAPPIEST ELF on 6/9/2017.
 */

public class ViewPagerAdapter extends PagerAdapter{

    private Context context;
    private LayoutInflater layoutInflater;
    private Integer[] images = {R.drawable.bacon_cheddar, R.drawable.marionberry, R.drawable.brownie};


    @Override
    public int getCount(){
        return 0;
    }

    @Override
    public boolean isViewFromObject(View view, Object object){
        return view == object;
    }
}

package com.mac2corvallis.mac2uconcepts;

import android.app.ActivityOptions;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.view.ViewPager;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.transition.Slide;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity
        implements NavigationView.OnNavigationItemSelectedListener{
    ViewPager viewPager;
    public void onAttachedToWindow() {
        super.onAttachedToWindow();
        Window window = getWindow();
        window.setFormat(PixelFormat.RGBA_8888);

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        //Log.d("My Tag:","On Creation");
        getWindow().requestFeature(Window.FEATURE_CONTENT_TRANSITIONS);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_activity);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, toolbar,R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.setDrawerListener(toggle);
        toggle.syncState();
        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);

        viewPager = (ViewPager) findViewById(R.id.viewPager)

        setupWindowAnimations();
        getWindow().setAllowEnterTransitionOverlap(false);
    }


    private void setupWindowAnimations(){
        Log.d("My Tag","Setup Window Animations");
        Slide slideTransition = new Slide();
        getWindow().setReenterTransition(slideTransition);
        getWindow().setExitTransition(slideTransition);

        getWindow().setAllowReturnTransitionOverlap(false);
    }
    public void slideTransitionByCode(View view){
        ActivityOptions options = ActivityOptions.makeSceneTransitionAnimation(this);
        Intent i = new Intent(MainActivity.this, Create_New_Account.class);
        i.putExtra(Constants.KEY_ANIM_TYPE,Constants.TransitionType.SLideJava);
        i.putExtra(Constants.KEY_TITLE,"MAC 2 U");
        startActivity(i, options.toBundle());

    }
    public void slideTransitionByXML(View view){
        ActivityOptions options = ActivityOptions.makeSceneTransitionAnimation(this);
        Intent intent = new Intent(MainActivity.this, Create_New_Account.class);
        intent.putExtra(Constants.KEY_ANIM_TYPE,Constants.TransitionType.SLideXML);
        intent.putExtra(Constants.KEY_TITLE,"Slide By XML");
        startActivity(intent, options.toBundle());
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu){
      //  Log.d("My Tag","Create Options Menu");
        MenuInflater menuInflater = getMenuInflater();
        menuInflater.inflate(R.menu.menu_bar,menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item){
        Log.d("My Tag", "in onOptionsSelected");
        int res_id = item.getItemId();
        if(res_id == R.id.shopping_cart){
            Toast.makeText(getApplicationContext(),"you selected shopping cart", Toast.LENGTH_LONG).show();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onBackPressed(){
        Log.d("My Tag","Back has been pressed");
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        if(drawer.isDrawerOpen(GravityCompat.START)){
            drawer.closeDrawer(GravityCompat.START);
        }
        else{
            super.onBackPressed();
        }
    }
    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item){
        Log.d("My Tag", "navigationItemSelected");
        int id = item.getItemId();
        if(id==R.id.nav_account){
            ActivityOptions options = ActivityOptions.makeSceneTransitionAnimation(this);
            Intent i = new Intent(MainActivity.this, Create_New_Account.class);
            i.putExtra(Constants.KEY_ANIM_TYPE, Constants.TransitionType.SLideJava);
            i.putExtra(Constants.KEY_TITLE, "Slide By Java Code");
            startActivity(i, options.toBundle());
        }
        else if(id==R.id.nav_contact){

        }
        else if(id==R.id.nav_about){

        }
        else if(id==R.id.nav_sign_out){

        }
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;
    }
}

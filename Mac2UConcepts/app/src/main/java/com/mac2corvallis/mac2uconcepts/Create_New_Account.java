package com.mac2corvallis.mac2uconcepts;

import android.app.ActivityOptions;
import android.content.Intent;
import android.os.Bundle;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.transition.Slide;
import android.transition.Transition;
import android.transition.TransitionInflater;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;
import android.view.animation.AnticipateOvershootInterpolator;
import android.widget.Toast;

/**
 * Created by THE HAPPIEST ELF on 5/23/2017.
 */

public class  Create_New_Account extends AppCompatActivity implements NavigationView.OnNavigationItemSelectedListener{
    Constants.TransitionType type;
    String toolbarTitle;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().requestFeature(Window.FEATURE_CONTENT_TRANSITIONS);

        super.onCreate(savedInstanceState);
        setContentView(R.layout.new_account);
        Log.d("myTag","OnCreate Success new account");
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbaraccount);
        setSupportActionBar(toolbar);
        initPage();
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.setDrawerListener(toggle);
        toggle.syncState();

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);
        initAnimation();

        getWindow().setAllowEnterTransitionOverlap(false);
    }
    private void initPage(){
        type = (Constants.TransitionType) getIntent().getSerializableExtra(Constants.KEY_ANIM_TYPE);
        toolbarTitle = getIntent().getExtras().getString(Constants.KEY_TITLE);
    }

    @Override
    public boolean onSupportNavigateUp(){
        finishAfterTransition();
        return true;
    }
    private void initAnimation(){
        type = (Constants.TransitionType) getIntent().getSerializableExtra(Constants.KEY_ANIM_TYPE);
        switch(type){
            case SLideJava:{
                Slide enterTransition = new Slide();
                enterTransition.setSlideEdge(Gravity.BOTTOM);
                enterTransition.setDuration(1000);
                enterTransition.setInterpolator(new AnticipateOvershootInterpolator());
                getWindow().setEnterTransition(enterTransition);
                break;

            }
            case SLideXML:{
                Transition enterTransition = TransitionInflater.from(this).inflateTransition(R.transition.slide);
                getWindow().setEnterTransition(enterTransition);
                break;
            }

        }
    }
    @Override
    public void onBackPressed(){
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        if(drawer.isDrawerOpen(GravityCompat.START)){
            drawer.closeDrawer(GravityCompat.START);
        }
        else{
            super.onBackPressed();
        }

    }
    @Override
    public boolean onCreateOptionsMenu(Menu menu){
        Log.d("My Tag","n here jared Options Menu");
        getMenuInflater().inflate(R.menu.menu_bar,menu);
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

    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item){
        Log.d("My Tag", "navigationItemSelected");
        int id = item.getItemId();
        if(id==R.id.nav_account){
            ActivityOptions options = ActivityOptions.makeSceneTransitionAnimation(this);
            Intent i = new Intent(Create_New_Account.this, Create_New_Account.class);
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

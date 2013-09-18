#include "dgame.h"

template<size_t N>
class mvmap_c : public map_c<N>
{
  public:
    vector< mvregion_c<N> > obstacles;

    mvmap_c() : map_c<N> () {}
    void insert_obstacles(vector< mvregion_c<N> >& obstacles_in)
    {
      obstacles = obstacles_in;
    }
    bool is_in_collision(const float s[N])
    {
      for(auto& r : obstacles)
      {
        if(r.is_inside(s))
        {
          //cout<<"returning true"<<endl;
          return true;
        }
      }
      return false;
    }
};



void example1()
{
  typedef mvsystem_c<double_integrator_c, mvmap_c<4>, mvregion_c<4>, cost_c<4>, automaton_product_c<2> > system_t;
  //typedef mvsystem_c<single_integrator_c<4>, mvmap_c<4>, mvregion_c<4>, cost_c<1>, automaton_product_c<-1> > system_t;
  typedef system_t::state state;
  typedef typename system_t::control control;
  typedef typename system_t::trajectory trajectory;
  typedef typename system_t::region_t region;

  lcm_t *lcm          = bot_lcm_get_global(NULL);
  bot_lcmgl_t *lcmgl  = bot_lcmgl_init(lcm, "plotter");
  bot_lcmgl_switch_buffer(lcmgl);

  float w = 5;
  float maxv = 1;
  float nomv = 0.5;
  float epsilon = 0.01;

  vector<pair<size_t, automaton_ss_c> > rules;
  rules.push_back( make_pair(0, automaton_ss_c(false, SIDEWALK, 1, 0)));
  rules.push_back( make_pair(1, automaton_ss_c(true, GOOD_DIR, 1, 0)));
  rules.push_back( make_pair(2, automaton_ss_c(false, SLOW, 1, 0)));

  float zero[4] = {0};
  float size[4] = {3*w,3*w,2*maxv,2*maxv};
  region op_region = region(zero, size);

  float sc1[4] = {w/4,-3*w/2,0, nomv};
  float gc1[4] = {-3*w/2,w/4, -nomv,0};
  float gs[4] = {0.2,0.2,0.1,0.1};

  float sc2[4] = {-3*w/2,-w/4, nomv,0};
  float gc2[4] = {w/4,3*w/2,0, nomv};

  region gr1 = region(gc1, gs);
  region gr2 = region(gc2, gs);
  region sr1 = region(sc1, gs);
  region sr2 = region(sc2, gs);

  vector<region> regions;
#if 1
  //rcrr = region_center_right-lane_right-hand-side
  // note : for labeled regions, the center for velocity
  //        coordinates is really just the direction
  
  float de1[2] = {1, 0};
  float de2[2] = {0, 1};
  float de3[2] = {-1,0};
  float de4[2] = {0,-1};

  float rcrd[4] = {w/4, -w, 0, epsilon};
  float rcru[4] = {w/4, w, 0, epsilon};
  float rcld[4] = {-w/4, -w, 0,-epsilon};
  float rclu[4] = {-w/4, w, 0,-epsilon};

  float rcswrd[4] = {w,-w,0,0};
  float rcswru[4] = {w,-w,0,0};
  float rcswld[4] = {-w,-w,0,0};
  float rcswlu[4] = {-w,-w,0,0};

  float rcrr[4] = {w, -w/4, epsilon,0};
  float rcrl[4] = {-w, w/4, -epsilon,0};
  float rclr[4] = {w, w/4, -epsilon,0};
  float rcll[4] = {-w, -w/4, epsilon,0};

  float rsx[4] = {w, w/2, 2*maxv, 2*maxv};
  float rsy[4] = {w/2, w, 2*maxv, 2*maxv};

  float rssw[4] = {w,w, 2*maxv, 2*maxv};

  label_c right_lane(RIGHT_LANE);
  label_c left_lane(LEFT_LANE);
  label_c sidewalk(SIDEWALK);

  regions.push_back(region(rcrd, rsy, de2, right_lane));
  regions.push_back(region(rcru, rsy, de2, right_lane));
  regions.push_back(region(rcld, rsy, de4, left_lane));
  regions.push_back(region(rclu, rsy, de4, left_lane));

  regions.push_back(region(rcrr, rsx, de1, right_lane));
  regions.push_back(region(rcrl, rsx, de1, right_lane));
  regions.push_back(region(rclr, rsx, de3, left_lane));
  regions.push_back(region(rcll, rsx, de3, left_lane));

  /*
     regions.push_back(region(rcswrd, rssw, sidewalk));
     regions.push_back(region(rcswru, rssw, sidewalk));
     regions.push_back(region(rcswld, rssw, sidewalk));
     regions.push_back(region(rcswlu, rssw, sidewalk));
     */
#endif

  dgame_c<system_t> dgame(lcmgl);
  dgame.insert_rules(rules);
  dgame.initialize(op_region, sr1, gr1, sr2, gr2, regions);

  vector<region> obstacles;
  float ocrd[4] = {w,-w, 0, 0};
  float ocru[4] = {w,w,0,0};
  float oclu[4] = {-w, w, 0,0};
  float ocld[4] = {-w, -w, 0,0};
  float os[4] = {w,w, 2*maxv,2*maxv};

  obstacles.push_back(region(ocrd, os)); 
  obstacles.push_back(region(ocru, os)); 
  obstacles.push_back(region(ocld, os)); 
  obstacles.push_back(region(oclu, os)); 

  dgame.insert_obstacles(obstacles);

  for(auto i : range(0, 3000))
  {
    dgame.iteration();
    if(i%100 == 0)
    {
      dgame.draw();
      //getchar();
    }
    if(i%10 == 0)
      cout<<i<<endl;
  }
  dgame.draw();

  return;
}

void example2()
{
  typedef mvsystem_c<single_integrator_c<2>, mvmap_c<2>, mvregion_c<2>, cost_c<4>, automaton_product_c<2> > system_t;
  typedef system_t::state state;
  typedef typename system_t::control control;
  typedef typename system_t::trajectory trajectory;
  typedef typename system_t::region_t region;

  lcm_t* lcm          = bot_lcm_get_global(NULL);
  bot_lcmgl_t* lcmgl  = bot_lcmgl_init(lcm, "plotter");
  bot_lcmgl_switch_buffer(lcmgl);

  float w = 3;
  float epsilon = 0.1;

  vector<pair<size_t, automaton_ss_c> > rules;
  rules.push_back( make_pair(0, automaton_ss_c(false, SIDEWALK, 1, 0)));
  rules.push_back( make_pair(1, automaton_ss_c(true, GOOD_DIR, 1, 0)));
  rules.push_back( make_pair(2, automaton_ss_c(false, LANE_CHANGE, 0, 1)));
  
  float zero[2] = {0};
  float size[2] = {3*w,3*w};
  region op_region = region(zero, size);

  float sc1[2] = {w/4,-3*w/2};
  float gc1[2] = {-3*w/2,w/4};
  float gs[2] = {0.1,0.1};

  float sc2[2] = {-3*w/2,-w/4};
  float gc2[2] = {w/4,3*w/2};

  region gr1 = region(gc1, gs);
  region gr2 = region(gc2, gs);
  region sr1 = region(sc1, gs);
  region sr2 = region(sc2, gs);

  vector<region> regions;
  
  float de1[2] = {1, 0};
  float de2[2] = {0, 1};
  float de3[2] = {-1,0};
  float de4[2] = {0,-1};

  float rh[2] = {0,-w/4};
  float lh[2] = {0,w/4};

  float rv[2] = {w/4,0};
  float lv[2] = {-w/4,0};

  float sh[2] = {3*w, w/2};
  float sv[2] = {w/2, 3*w};

  label_c right_lane(RIGHT_LANE);
  label_c left_lane(LEFT_LANE);
  label_c sidewalk(SIDEWALK);

  regions.push_back(region(rh, sh, de1, right_lane));
  regions.push_back(region(rv, sv, de2, right_lane));
  
  regions.push_back(region(lh, sh, de3, left_lane));
  regions.push_back(region(lv, sv, de4, left_lane));

  dgame_c<system_t> dgame(lcmgl);
  dgame.insert_rules(rules);
  dgame.initialize(op_region, sr1, gr1, sr2, gr2, regions);
  
  vector<region> obstacles;
  float ocrd[2] = {w,-w};
  float ocru[2] = {w,w};
  float oclu[2] = {-w, w};
  float ocld[2] = {-w, -w};
  float os[2] = {w,w};

  obstacles.push_back(region(ocrd, os)); 
  obstacles.push_back(region(ocru, os)); 
  obstacles.push_back(region(ocld, os)); 
  obstacles.push_back(region(oclu, os)); 

  dgame.insert_obstacles(obstacles);

  for(auto i : range(0, 10000))
  {
    dgame.iteration();
    if(i%100 == 0)
    {
      cout<<i<<endl;
      dgame.draw(i);
      //getchar();
    }
  }
  dgame.draw();

  return;
}


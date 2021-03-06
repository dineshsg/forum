<?php

    function forum()
    {
	    global $user;
		$output ="";
	  	if(in_array('administrator',$user->roles) || in_array('admin moderator',$user->roles) || in_array('moderator',$user->roles)|| in_array('admin moderator',$user->roles)){
	 		$output.="<div class='col-12 text_right'><a href='forum/question/approval' class='btn btn-default content_down'>Approve Questions</a> <a href='forum/comment/approval' class='btn btn-default content_down'>Approve Answers</a></div>";
		}
		$output.="<h1 class='font-500 align-center'>Welcome</h1><br>";
        $search = drupal_get_form("forum_search");
        $output .= "<div class='col-lg-3 col-md-2 col-sm-1 col-xs-0'>&nbsp;</div><div class='col-lg-7 col-md-8 col-sm-10 col-xs-12'><span>".drupal_render($search)."</span></div><div class='col-lg-2 col-md-2 col-sm-1 col-xs-0'>&nbsp;</div><div class='clear'></div>";
        $query = 'SELECT DISTINCT fg.grade_name,fg.id FROM forum_grades fg, forum_subjects fs WHERE fg.id = fs.grade_id';
        $result = db_query($query);
        
        $count = 0;
		foreach ($result as $row)
        {
			 $img=$row->grade_name."png";
			
			switch($row->grade_name)
			{
					case "Pre-primary": 
									$img="primary.png";
									break;
					case "Junior Primary" : 
									$img="JuniorPrimary.png";
									break;
					case "Senior Primary" : 
									$img="SeniorPrimary.png";
									break;	
					case "High School" : 
									$img="HighSchool.png";
									break;	
					/*default : $img=$row->grade_name."png";*/
					default : $img="common.png";
												
			}			
				
            if($count)
            {			
			    $output .= "<div class='grade1 common' id= '".$row->id."' target='".$count."'>
                            <div class='thumb' style='background-image:url(./images/grades/".$img.");cursor:pointer;'>        	
        	                &nbsp; 
                            </div><div class='caption content-font common' id='title".$row->id."'  target='".$count."' style='cursor:pointer;'>".$row->grade_name."</div></div>";
            }
            else
            {		
			    $output .= "<div class='grade1 common' id= '".$row->id."' target='".$count."'>
			            	<div class='thumb' style='background-image:url(./images/grades/".$img.");cursor:pointer;' >       	
			               	&nbsp; 
            				</div><div class='caption content-font common' id='title".$row->id."'  target='".$count."' style='cursor:pointer;'>".$row->grade_name."
			            	</div></div>";
            }
            $count++;		
        }
		
        $output .= "<div class='container_subjects col-12' style='clear:both;
                    overflow:hidden;padding-top:15px;' id='container_subjects'></div>"; //background:#efefef;! important;
        return $output;
    }
    
    function forum_search($form, &$form_state)
    {
        $form = array();		
        $form['search'] = array(
		    '#type' => 'textfield',
		    '#size' => 30,
		    '#required' => TRUE,
		);
		$form["submit"] = array(
            '#type' => 'submit',
            '#value' => "Search",
        );
        return $form;
    }
    function forum_search_submit($form, &$form_state)
    {
        $redirect = 'forum/search/result/?param='.$form_state['values']['search'];
        drupal_goto($path = $redirect);
    }
    
    function forum_search_result($param)
    {
        $output = '';
        $params = $param;
        if($params=='NULL')
        {
           // drupal_goto('forum/search');
		    drupal_goto('forum');
        }
        else
        {
            $params = explode('=', $param);
            $header = array(
                array('data' => 'Grade', 'field' => 'subject_name', 'sort' => 'desc'),
                array('data' => 'Topic', 'field' => 'grade_name', 'sort' => 'desc'),
                array('data' => 'Question', 'field' => 'question', 'sort' => 'desc'),
                array('data' => 'Posted By', 'field' => 'created', 'sort' => 'desc'),
            );
            $result = db_select('forum_questions','fq');
            $result->join('forum_grades','fg', 'fq.grade_id = fg.id');
            $result->join('forum_subjects','fs', 'fq.subject_id = fs.id');
            $result->join('forum_question_reply','fqr', 'fqr.question_id = fq.id');
            $result->fields('fs', array('subject_name'));
            $result->fields('fg', array('grade_name'));
            $db_or = db_or();
            $db_or->condition('fg.grade_name', '%' . db_like($params[1]) . '%', 'LIKE');
            $db_or->condition('fs.subject_name', '%' . db_like($params[1]) . '%', 'LIKE');
            $db_or->condition('fq.question', '%' . db_like($params[1]) . '%', 'LIKE');        
            $result->fields('fq', array('question','user_id','created','id'));
            $result->condition('fqr.status', 1, '=');
            $result->condition('fq.status', 1, '=')
			       ->condition('fqr.status', 1, '=')
			       ->condition($db_or)
			       ->execute()
                   ->fetchAll();
		  if(isset($_REQUEST['sort']))
		  {
				$table_sort = $result->extend('TableSort')							
						  		 ->orderByHeader($header);
			    $pager = $table_sort->extend('PagerDefault') 
								 ->limit(10); 
                  				 /*->limit(5);*/
		  }else{
          		 $pager = $result->extend('PagerDefault')                  
          		                 ->orderBy('id', 'DESC')
								 ->limit(10); 
                  	             /*->limit(5);*/
		  }
            $result = $pager->execute();
            if($result->rowCount() > 0)
	        {
	            while ($item = $result->fetch())
                {
                    $query_user = user_load($item->user_id);
                    $query_username = $query_user->name;
                    $date_ago = ago($item->created);
                    $row[] = array($item->grade_name,$item->subject_name,l(".$item->question.",'forum/question-detail/'.$item->id.''),$query_username.'<div class="algn-right small-font">'.$date_ago.'</div>');
                }
                $form_search = drupal_get_form('forum_search');
                $output .= drupal_render($form_search);
                $output .= theme('table',array('header' => $header,'rows' => $row));
                $output .= theme('pager');
			    $baseurl= base_path()."ask-a-question";
			    $output .= "<div class='content_down clear empty_content'><a href='".$baseurl."' class='btn btn-default' >Ask a question</a></div>";
           }
           else
	       {
			    $baseurl1=base_path()."forums";
			    $baseurl=base_path()."ask-a-question";
	        	/*$output .= "<div class='sub-title clear'>Invalid Grade and Topic.</div><div class='col-12 content_down'><a class='btn btn-default' href='".$baseurl1."'>Back to Forum</a>&nbsp;<a href='".$baseurl."' class='btn btn-default' >Ask a question</a></div>";*/
				$output .= "<div class='sub-title clear'>No search results found</div><div class='col-12 content_down'><a class='btn btn-default' href='".$baseurl1."'>Back to Forum</a>&nbsp;<a href='".$baseurl."' class='btn btn-default' >Ask a question</a></div>";
	       }
	   
        return $output;
        }
    }
?>

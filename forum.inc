<?php

    function forum()
    {
	global $user;
		$output ="";
	  	if(in_array('administrator',$user->roles) || in_array('admin moderator',$user->roles) || in_array('moderator',$user->roles)){
	 		$output.="<div class='col-12 text_right'><a href='forum/question/approval' class='btn btn-default'>Approve Question</a> <a href='forum/comment/approval' class='btn btn-default'>Approve Answers</a></div>";
		}
		$output.="<h1 class='font-500 align-center'>Welcome</h1><br>";
        $search = drupal_get_form("forum_search");
        $output .= "<div class='col-lg-3 col-md-2 col-sm-1 col-xs-0'>&nbsp;</div><div class='col-lg-7 col-md-8 col-sm-10 col-xs-12'><span>".drupal_render($search)."</span></div><div class='col-lg-2 col-md-2 col-sm-1 col-xs-0'>&nbsp;</div><div class='clear'></div>";
        $query = 'SELECT DISTINCT fg.grade_name,fg.id FROM forum_grades fg, forum_subjects fs WHERE fg.id = fs.grade_id';
        $result = db_query($query);
        
        $count = 0;
        /*foreach ($result as $row)
        {
            //var_dump($row->grade_name);
            if($count)
            {
            $output .= "<div class='grade common' id='".$row->id."' target='".$row->id."'>
            <div class='thumb'>
        	<img data-src='holder.js/225x100/random/font:Droid Sans/text:".$row->grade_name."'>
        	<div class='caption'>
        		Placeholder with random theme
        	</div>
            </div>
            </div>";
            }
            else
            {
            $output .= "<div class='grade1 common' id= '".$row->id."' target='".$row->id."'>
            <div class='thumb'>
        	<img data-src='holder.js/225x100/random/font:Droid Sans/text:".$row->grade_name."'>
        	<div class='caption'>
        		Placeholder with random theme
        	</div>
            </div>
            </div>";
            }
            //$output .=".$row->subject_name.";
            $count++;
        }
        $output .= "<div class='container_subjects' style='clear:both;background:#efefef;! important;height:191px;width:96%;' ></div>";*/
		foreach ($result as $row)
        {
            //var_dump($row->grade_name);
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
					default : $img=$row->grade_name."png";
												
				}
            if($count)
            {
            /*$output .= "<div class='grade common' id='".$row->id."' target='".$row->id."'><div class='thumb'><img data-src='holder.js/225x100/random/font:Droid Sans/text:".$row->grade_name."'>
        	<div class='caption'>Placeholder with random theme</div></div></div>";	*/
			
			$output .= "<div class='grade1 common' id= '".$row->id."' target='".$count."'>
            <div class='thumb' style='background-image:url(./images/grades/".$img.");cursor:pointer;'>        	
        	&nbsp; 
            </div><div class='caption content-font common' id='title".$row->id."'  target='".$count."' style='cursor:pointer;'>".$row->grade_name."</div></div>";
            }
            else
            {
          /*$output .= "<div class='grade1 common' id= '".$row->id."' target='".$row->id."'>*/
		
			$output .= "<div class='grade1 common' id= '".$row->id."' target='".$count."'>
				<div class='thumb' style='background-image:url(./images/grades/".$img.");cursor:pointer;' >        	
				&nbsp; 
				</div><div class='caption content-font common' id='title".$row->id."'  target='".$count."' style='cursor:pointer;'>".$row->grade_name."
				</div></div>";
			//<img data-src='holder.js/225x100/random/font:Droid Sans/text:".$row->grade_name."'>
            }
            //$output .=".$row->subject_name.";
			//$output .="<input type='hidden' id='hdn".$row->id."' name='".$count."' value='".$count."' />";
            $count++;		
        }
		
        /* $output .= "<div class='container_subjects' style='clear:both;background:#efefef;! important;width:96%;
overflow:hidden;padding-left:auto;padding-right:auto;' id='container_subjects'  ></div>";*///height:191px;  id='".$divid."'

 $output .= "<div class='container_subjects col-12' style='clear:both;background:#efefef;! important;
overflow:hidden;padding-top:15px;' id='container_subjects'  ></div>";
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
        $result->fields('fq', array('question','user_id','created','id'));
        $result->condition('fqr.status', 1, '=');
        $result->condition('fq.status', 1, '=')
			   ->condition('fqr.status', 1, '=')
			   ->condition('fq.question', '%' . db_like($params[1]) . '%', 'LIKE')->distinct()
			   ->execute()
               ->fetchAll();
        $table_sort = $result->extend('TableSort')
                    ->orderByHeader($header);
        $pager = $table_sort->extend('PagerDefault')
               ->orderBy('created', 'DESC')
               ->limit(5);
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
			$baseurl=base_path()."ask-a-question";
			$output .= "<div class='content_down clear empty_content'><a href='".$baseurl."' class='btn btn-default' >Ask a question</a></div>";
       }
       else
	   {
			$baseurl1=base_path()."forums";
			$baseurl=base_path()."ask-a-question";
	    	$output .= "<div class='sub-title clear'>Invalid Grade and Topic.<a class='btn btn-default' href='".$baseurl1."'>Go Back to Forum</a></div><br /><a href='".$baseurl."' class='btn btn-default' >Ask a question</a>";
	   }
        return $output;
    }
?>

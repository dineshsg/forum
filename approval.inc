<?php
	/*-------------------------------------------
        Questions waiting for Moderators Approval
    ----------------------------------------------*/
    function question_waiting_for_approval()
    {
        global $user;
        $q_id = arg(3);
        $q_status = arg(4);
        if(($q_status==1)||($q_status==-1))
        {
            if($q_status==1)
            {
				/*---------------------------------------------
                    Question which are approved by the Moderator
                ------------------------------------------------*/
                $query_app = "UPDATE `forum_questions` SET `status`='".$q_status."', `updated` = CURRENT_TIMESTAMP WHERE id = '".$q_id."'";            
                if(db_query($query_app)){
                    drupal_set_message('The question has been approved');
                }else {
                    drupal_set_message('Error !!!!.Please try again.','error');
                }
				
				///---- Start Code added by Manjiri	
				$stu_query = db_query("SELECT user_id,grade_id,subject_id,question FROM forum_questions WHERE id = ".$q_id)->fetchObject();
				//$stu_query->grade_id
				$role_query=db_query("select rid from role where name='teacher'")->fetchObject();
				$getteacher_query=db_query("select uid from users_roles where rid=".$role_query->rid);					
				
                
				//$message = array('0'=>t('Dear Teacher,<br><br>A question has been posted on forums and is waiting for your reply<br> ')); // Body of your email here.
				$file_attachment = db_query("SELECT file_name FROM forum_question_attach WHERE question_id = ".$q_id);
				$output = '';
				while($link = $file_attachment->fetchObject())
				{
					$output .="<br>". l('Click Here To Download', $link->file_name, array('absolute' => TRUE));
				}
				if($output =='')
					$output='NA';				
				
								$message = array('0'=>t('<span class="content-font">Dear Teacher,<br><br>A question has been posted on forums and is waiting for your reply<br>
                                         Grade: '.grade($stu_query->grade_id).'<br>
                                         Topic: '.subject($stu_query->subject_id).'<br>
                                         Question: '.$stu_query->question.'<br> 
										 File Attachment : '.$output.'</span>')); //// Body of your email here.

                $params = array(
                    'body' => $message,
                    'subject' => 'Question waiting for Reply',
                );
                $teacher_mail_id = role_mail('teacher',$stu_query->grade_id,$stu_query->subject_id);
			    $to = $teacher_mail_id;
			   // $to="dineshsgaddi@gmail.com";
                $language = user_preferred_language($user);
                drupal_mail('forum', 'postedquestion', $to, $language, $params, 'demo@demo.com', $send = TRUE);
                drupal_goto('forum/question/approval');
            }
            else
            {
                $query_rej = "UPDATE `forum_questions` SET `status`='".$q_status."', `updated` = CURRENT_TIMESTAMP WHERE id = '".$q_id."'";            
                if(db_query($query_rej)){
                    drupal_set_message('Question has been Rejected');
                }else {
                    drupal_set_message('Error !!!!.Please try again.','error');
                }

                $stu_query = db_query("SELECT user_id,grade_id,subject_id,question FROM forum_questions WHERE id = ".$q_id)->fetchObject();
                $student_user = user_load($stu_query->user_id);
                $file_attachment = db_query("SELECT file_name FROM forum_question_attach WHERE question_id = ".$q_id);
				$output = '';
				while($link = $file_attachment->fetchObject())
				{
					$output .="<br>". l('Click Here To Download', $link->file_name, array('absolute' => TRUE));
					//$output .="<a href='.$link->file_name.' > Click Here To Download</a>";
				}
				 // Body of your email here
				if($output)
				{
                $message = array('0'=>t('<span class="content-font">Dear Student,<br><br>A question posted by you on forums has been rejected by the moderator.<br>
                                 Grade: '.grade($stu_query->grade_id).'<br>
                                 Topic: '.subject($stu_query->subject_id).'<br>
                                 Question: '.$stu_query->question.'<br>
								 File Attachment : '.$output.'</span>')); 
                }
                else
                {
                $message = array('0'=>t('<span class="content-font">Dear Student,<br><br>A question posted by you on forums has been rejected by the moderator.<br>
                                 Grade: '.grade($stu_query->grade_id).'<br>
                                 Topic: '.subject($stu_query->subject_id).'<br>
                                 Question: '.$stu_query->question.'<br></span>
								 ')); 
                }
                $params = array(
                    'body' => $message,
                    'subject' => 'Question has been Rejected by the Moderator',
                );
                $to = $student_user->mail;			   
                $language = user_preferred_language($user);
                drupal_mail('forum', 'rejectaquestion', $to, $language, $params, 'demo@demo.com', $send = TRUE);
                drupal_goto('forum/question/approval');
            }
            
        }
        else
        {
            if ((in_array('moderator', $user->roles)) || (in_array('admin moderator', $user->roles)) || ($user->uid == 1))
            {
                $output = '';
                $search = drupal_get_form("forum_question_search");
                $output .= "<div class='col-lg-3 col-md-2 col-sm-1 col-xs-0'>&nbsp;</div><div class='col-lg-7 col-md-8 col-sm-10 col-xs-12'><span>".drupal_render($search)."</span></div><div class='col-lg-2 col-md-2 col-sm-1 col-xs-0'>&nbsp;</div><div class='clear'></div>";
                $row = array();
                $header = array(
                            array('data' => 'Grade', 'field' => 'subject_name', 'sort' => 'asc'),
                            array('data' => 'Topic', 'field' => 'grade_name', 'sort' => 'asc'),
                            array('data' => 'Question', 'field' => 'question', 'sort' => 'asc'),
							array('data' => 'File', 'field' => 'id', 'sort' => 'asc'),
                            array('data' => 'Posted By', 'field' => 'user_id', 'sort' => 'asc'),
                            array('data' =>'Action', 'field' => '', 'sort' => 'asc'),							
                            );
				if ((in_array('administrator', $user->roles)))
				{
					array_push($header,array('data' =>'Status', 'field' => '', 'sort' => 'asc'));
				}
                $result = db_select('forum_questions','fq');
                $result->join('forum_grades','fg', 'fq.grade_id = fg.id');
                $result->join('forum_subjects','fs', 'fq.subject_id = fs.id');
                $result->fields('fs', array('subject_name'));
                $result->fields('fg', array('grade_name'));
                $result->fields('fq', array('question','user_id','created','id','status'));
				 if ((in_array('administrator', $user->roles)))
				 {
				 	$db_or = db_or();
					$db_or->condition('fq.status', '0' , '=');
					$db_or->condition('fq.status', '-1' , '=');
					$result->condition($db_or);
					
				 }
				 else
				  	$result->condition('fq.status',0,'=');
				 
                if (in_array('moderator', $user->roles))
                {
                    $moderator_query = db_query('SELECT * FROM users_extend WHERE uid = '.$user->uid)->fetchObject();
                    $myArray = explode(',', $moderator_query->subject);
                    $result->condition('fq.subject_id', $myArray, 'IN');
                    $result->condition('status', 0)
                           ->execute()
                           ->fetchAll();
                }
                else if(in_array('admin moderator', $user->roles))
                {
                    $result->condition('status', 0)
                           ->execute()
                           ->fetchAll();
                }
                else
                {
                }
                if(isset($_REQUEST['sort']))
		        {
				    $table_sort = $result->extend('TableSort')							
					                     ->orderByHeader($header);
			        $pager = $table_sort->extend('PagerDefault') 
										->orderBy('status', 'DESC') 
										->limit(10); 
                  		               /* ->limit(5);*/
                }else{
          		    $pager = $result->extend('PagerDefault')                  
          		                    ->orderBy('id', 'DESC')
									->orderBy('status', 'DESC')
									->limit(10); 
                  	               /* ->limit(5);*/
		        }
                $result = $pager->execute();
                if(!$result->rowCount())
                {
                    $output .= "No Questions for approval!!";
                }
                else
                {
                    while ($item = $result->fetch())
                    {
						$temp_user_post=user_load($item->user_id);
						$post_user=$temp_user_post->name;
                        $date_ago = ago($item->created);
						$attacment_query = db_query('SELECT question_id, file_name FROM forum_question_attach WHERE question_id = '.$item->id);
						$link="";$cnt=0;
						while ($row_attachment = $attacment_query->fetchObject())
						{					
							if($row_attachment->file_name)
								$link.=" <a href='".base_path().$row_attachment->file_name."' target='_blank'>File".++$cnt."</a>";						
						}
						if($link=="")
							$link="-";
						
						
						if ((in_array('administrator', $user->roles)))
						{
							if($item->status==-1)						
								$status="Rejected";						
							else
								$status="Pending";
							$row[] = array($item->grade_name,$item->subject_name,'<a href="../question-det-approval/'.$item->id.'/" title="Click here to view question details">'.truncate_chars($item->question,50).'</a>',$link, $post_user.'<div class="algn-right small-font">'.$date_ago.'</div>',l('Accept','forum/question-det-approval/'.$item->id.'/1',array('attributes' => array('title' => 'Click here to accept question'))).' | '.l('Reject','forum/question/approval/'.$item->id.'/-1',array('attributes' => array('class' => 'reject','title' => 'Click here to reject question'))),$status);		
						}
						else{
							$row[] = array($item->grade_name,$item->subject_name,'<a href="../question-det-approval/'.$item->id.'/" title="Click here to view question details">'.truncate_chars($item->question,50).'</a>',$link, $post_user.'<div class="algn-right small-font">'.$date_ago.'</div>',l('Accept','forum/question-det-approval/'.$item->id.'/1',array('attributes' => array('title' => 'Click here to accept question'))).' | '.l('Reject','forum/question/approval/'.$item->id.'/-1',array('attributes' => array('class' => 'reject','title' => 'Click here to reject question'))));
						}
                    }
                    $output .= theme('table',array('header' => $header,'rows' => $row));
                    $output .= theme('pager');
                }
    	    
            }
            else
            {
                drupal_set_message('You dont have access-rights to view this page.','warning');
                $output = '';
            }
            return $output;
        }
    }
/*------------------------------------------------------------------------------*/

function forum_question_search($form, &$form_state)
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

function forum_question_search_submit($form, &$form_state)
    {
        $redirect = 'forum/question/search/result/?param='.$form_state['values']['search'];
        drupal_goto($path = $redirect);
    }

function forum_question_search_result($param)
    {
        global $user;
        $output = '';
        $params = $param;
        if(empty($params))
        {
		    drupal_goto('forums');
        }
        else
        {
            $params = explode('=', $param);
            $search = drupal_get_form("forum_question_search");
            $output .= "<div class='col-lg-3 col-md-2 col-sm-1 col-xs-0'>&nbsp;</div><div class='col-lg-7 col-md-8 col-sm-10 col-xs-12'><span>".drupal_render($search)."</span></div><div class='col-lg-2 col-md-2 col-sm-1 col-xs-0'>&nbsp;</div><div class='clear'></div>";
            $row = array();
            $header = array(
                          array('data' => 'Grade', 'field' => 'subject_name', 'sort' => 'asc'),
                          array('data' => 'Topic', 'field' => 'grade_name', 'sort' => 'asc'),
                          array('data' => 'Question', 'field' => 'question', 'sort' => 'asc'),
					      array('data' => 'File', 'field' => 'id', 'sort' => 'asc'),
                          array('data' => 'Posted By', 'field' => 'user_id', 'sort' => 'asc'),
                          array('data' =>'Action', 'field' => '', 'sort' => 'asc'),							
                      );
            if ((in_array('administrator', $user->roles)))
			{
			    array_push($header,array('data' =>'Status', 'field' => '', 'sort' => 'asc'));
			}
            $result = db_select('forum_questions','fq');
            $result->join('forum_grades','fg', 'fq.grade_id = fg.id');
            $result->join('forum_subjects','fs', 'fq.subject_id = fs.id');
            $result->fields('fs', array('subject_name'));
            $result->fields('fg', array('grade_name'));
            $db_or = db_or();
            $db_or->condition('fg.grade_name', '%' . db_like($params[1]) . '%', 'LIKE');
            $db_or->condition('fs.subject_name', '%' . db_like($params[1]) . '%', 'LIKE');
            $db_or->condition('fq.question', '%' . db_like($params[1]) . '%', 'LIKE');        
            $result->fields('fq', array('question','user_id','created','id','status'));
			if ((in_array('administrator', $user->roles)))
			{
			    $db_or1 = db_or();
				$db_or1->condition('fq.status', '0' , '=');
				$db_or1->condition('fq.status', '-1' , '=');
				$result->condition($db_or1)
				       ->condition($db_or);
   		    }
			else
			{
			    $result->condition('fq.status',0,'=')
			           ->condition($db_or);
			}	 
            if (in_array('moderator', $user->roles))
            {
                $moderator_query = db_query('SELECT * FROM users_extend WHERE uid = '.$user->uid)->fetchObject();
                $myArray = explode(',', $moderator_query->subject);
                $result->condition('fq.subject_id', $myArray, 'IN');
                $result->condition('status', 0)
                       ->condition($db_or)
                       ->execute()
                       ->fetchAll();
            }
            else if(in_array('admin moderator', $user->roles))
            {
                $result->condition('status', 0)
                       ->condition($db_or)
                       ->execute()
                       ->fetchAll();
            }
            else
            {}
            if(isset($_REQUEST['sort']))
		    {
			    $table_sort = $result->extend('TableSort')							
				                     ->orderByHeader($header);
			    $pager = $table_sort->extend('PagerDefault') 
				                    ->orderBy('status', 'DESC') 
									->limit(10); 
            }
            else
            {
          	    $pager = $result->extend('PagerDefault')                  
          		                ->orderBy('id', 'DESC')
								->orderBy('status', 'DESC')
								->limit(10); 
		    }
            $result = $pager->execute();
            if(!$result->rowCount())
            {
                $output .= "No Questions for approval!!";
            }
            else
            {
                while ($item = $result->fetch())
                {
				    $temp_user_post=user_load($item->user_id);
					$post_user=$temp_user_post->name;
                    $date_ago = ago($item->created);
					$attacment_query = db_query('SELECT question_id, file_name FROM forum_question_attach WHERE question_id = '.$item->id);
					$link="";$cnt=0;
					while ($row_attachment = $attacment_query->fetchObject())
					{					
					    if($row_attachment->file_name)
						    $link.=" <a href='".base_path().$row_attachment->file_name."' target='_blank'>File".++$cnt."</a>";						
					}
					if($link=="")
					    $link="-";
					if ((in_array('administrator', $user->roles)))
					{
					    if($item->status==-1)						
						    $status="Rejected";						
						else
							$status="Pending";
						$row[] = array($item->grade_name,$item->subject_name,'<a href="'.base_path().'forum/question-det-approval/'.$item->id.'/" title="Click here to view question details">'.truncate_chars($item->question,50).'</a>',$link, $post_user.'<div class="algn-right small-font">'.$date_ago.'</div>',l('Accept','forum/question-det-approval/'.$item->id.'/1',array('attributes' => array('title' => 'Click here to accept question'))).' | '.l('Reject','forum/question/approval/'.$item->id.'/-1',array('attributes' => array('class' => 'reject','title' => 'Click here to reject question'))),$status);		
					}
					else
					{
					    $row[] = array($item->grade_name,$item->subject_name,'<a href="'.base_path().'forum/question-det-approval/'.$item->id.'/" title="Click here to view question details">'.truncate_chars($item->question,50).'</a>',$link, $post_user.'<div class="algn-right small-font">'.$date_ago.'</div>',l('Accept','forum/question-det-approval/'.$item->id.'/1',array('attributes' => array('title' => 'Click here to accept question'))).' | '.l('Reject','forum/question/approval/'.$item->id.'/-1',array('attributes' => array('class' => 'reject','title' => 'Click here to reject question'))));
					}
                }
                $output .= theme('table',array('header' => $header,'rows' => $row));
                $output .= theme('pager');
            }
        return $output;
        }
    
    }
?>

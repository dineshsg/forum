<?php
	function ask_a_question_form($form_state)
	{
	    global $user;		
	    $subject_t = array();
		
	    $student_ask_q = db_query("SELECT grade,subject FROM users_extend WHERE uid=".$user->uid)->fetchObject();
		if(!($student_ask_q))
		{
		}
		else
		{
			$subjects = explode(',',$student_ask_q->subject);
			foreach($subjects as $temp_sub)
			{
				$subject_t[$temp_sub] = subject($temp_sub);
			}
	    }
		$option='';
		if($user->uid){
			$result_grade = db_query('SELECT id,grade_name FROM forum_grades') ->fetchAll();
			$grd_nm = $result_grade[0]->grade_name;
			$grd_id = $result_grade[0]->id;
			foreach ($result_grade as $node) {
				$option[$node->id] = $node->grade_name;
			}
      
		 $result_getuserinfo = db_query('SELECT grade,subject FROM users_extend WHERE uid='.$user->uid) ->fetchAll();		
		 if( $result_getuserinfo[0]->grade == 0 || $result_getuserinfo[0]->grade == '')
		 {
			//$grade=0;
			$grade=array(''=>'');
		 }
		 else{
			$grade=$result_getuserinfo[0]->grade;
			//array_push($stack, "apple", "raspberry");
		 }
		 
		 $subject_arr=explode(',',$result_getuserinfo[0]->subject);
		 if(!empty($subject_arr) && $subject_arr[0]!='0')
		 {		
			foreach($subject_arr as $subject_arr1){
				 $result_subject = db_query('SELECT subject_name FROM forum_subjects WHERE id='.$subject_arr1) ->fetchAll();		
				 $indx=$subject_arr1;
				 $subject[$indx]=$result_subject[0]->subject_name;
			}
		 }
		 else
		 {
			$subject=array(''=>'');
		 }
        $form['grade'] = array(
            '#title' => t('Grade'),
            '#type' => 'select',
			'#default_value' => $grade,
            '#options' => $option,
            '#attributes' => array('class' => array('grade','dropdown-forum')),
            '#validated' => TRUE,
            '#required' => TRUE,			
        );
        $form['subject'] = array(
            '#title' => t('Topic'),
            '#type' => 'select',
			'#options' => $subject,
            '#attributes' => array('class' => array('subject','dropdown-forum')),
            '#validated' => TRUE,
            '#required' => TRUE,
        );
        $form['file'] = array(
            '#type' => 'file',
            '#name' => 'files[]',
            '#title' => t('Attach Multiple Files'),
            '#description' => t('You are allow to upload jpg, jpeg, png and gif, 10MB Max Size.'),
            '#attributes' => array('multiple' => 'multiple','class' => array('dropdown-forum')),
            '#upload_validators' => array(
                'file_validate_size' => array(1024 * 1024),
            ),
        );
        $form['question_desc'] = array(
          '#title' => t('Ask your Question'),
          '#type' => 'textarea',
          '#required' => TRUE,
        );
        $form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Submit'),		
			 
	    );
        return $form;
        }
        else
        {
            drupal_set_message('You need to login to post a question <a href="node/53">Login</a>','warning');
        }
	}
    function ask_a_question_form_validate($form, $form_state)
    {
        global $user;

        if(($form_state['values']['grade'] == '')||($form_state['values']['grade'] == '-Select-')) { 
            form_set_error('grade', t('Please select a Grade.')); 
        }
        if(($form_state['values']['subject'] == '')||($form_state['values']['subject'] == '-Select-')) { 
            form_set_error('subject', t('Please select a Topic.')); 
        }
        if(($form_state['values']['grade'])&&($form_state['values']['subject'])&&(!in_array('administrator',$user->roles)))
        {
            $stu_grade_query = db_query("SELECT count(uid) as u_cnt,grade FROM users_extend WHERE uid = '".$user->uid."' AND grade = '".$form_state['values']['grade']."'")->fetchObject();
			$stu_subject_query = db_query("SELECT count(uid) as u_cnt,subject FROM users_extend WHERE uid = '".$user->uid."' AND FIND_IN_SET('".$form_state['values']['subject']."',subject)")->fetchObject();
			
            if(!$stu_grade_query->u_cnt || !$stu_subject_query->u_cnt)
            {
				if(!$stu_grade_query->grade && !$stu_subject_query->subject)
				{
					form_set_error('grade','You are not assigned to this particular Grade and Topic to post question');
					form_set_error('subject');
				}
				else
				{
					if(!$stu_grade_query->grade)
					{ 
						form_set_error('grade','You are not assigned to this particular Grade to post question');
					}
					else if(!$stu_subject_query->subject)
					{
						form_set_error('subject','You are not assigned to this particular Subject to post question');
					}
				}
            }
        }
        foreach($_FILES['files']['name'] as $key => $name)
        {
            $tmp_name = $_FILES['files']['tmp_name'][$key];
            if($tmp_name)
            {
                $name = $_FILES['files']['name'][$key];
                $tmp = explode(".", $name);
                $extension=end($tmp);
                $name="Attachment-".$key.".".$extension;
                $str = variable_get('exten_allowed', '');
                $allowedExts = explode(",",$str);
                if($tmp_name > 5242880){
                            form_set_error('file','File size is more than 10MB.');
                }
                if (!in_array($extension, $allowedExts)){
                    drupal_set_message(t('Some file cannot be uploaded on the forum. Please contact the administrator of this site.'), 'error');
                }
            }
        }
    }
	function ask_a_question_form_submit($form, &$form_state)
	{		
        global $user;
        $file_upload = 0;
	    $pre_text = $form_state['values']['question_desc'];
		$pre_text2 = htmlentities($form_state['values']['question_desc'],ENT_QUOTES);
	    $last = db_query("SELECT MAX(id) as last FROM forum_questions");
	    $last = $last->fetchObject();
	    $last = $last->last;
	    $last++;
	    if (!empty($_FILES['files']['tmp_name'][0])) 
	    {
	        $num_files = count($_FILES['files']['name']);
            $file_name = $_FILES['files']['name'];
            $directory = 'sites/default/attachment/'.$form_state['values']['grade'].'/'.$form_state['values']['subject'].'/'.$last;
            $str = variable_get('exten_allowed', '');
            $allowedExts = explode(",",$str);
            if(!file_exists($directory) && !is_dir($directory)) 
            {
                mkdir($directory , 0777, true);
                chmod($directory,0777);
            }
	        foreach($_FILES['files']['name'] as $key => $name)
            {
                $tmp_name = $_FILES['files']['tmp_name'][$key];
                if($tmp_name)
                {
                    $name = $_FILES['files']['name'][$key];
                    $tmp = explode(".", $name);
                    $extension=end($tmp);
                    $name="Attachment-".$key.".".$extension;
				
                    if (in_array($extension, $allowedExts)){
                        $moved = move_uploaded_file($tmp_name, $directory.'/'.$name);
                        $file_upload = 1;
                    }
                    else{
                            if($_FILES['files']['tmp_name'][$key]){
                            drupal_set_message(t($directory.'/'.$name.'Some file cannot be uploaded on the forum. Please contact the administrator of this site.'), 'error');
                            }
                    }
                    $path_name = $directory.'/'.$name;
                    $query = db_query("INSERT INTO `forum_question_attach` (`question_id`, `file_name` ,`file_type`) VALUES ('".$last."', '".$path_name."','".$_FILES['files']['type'][$key]."')");
                }
            }
        }
        
        $forum_question_query = "INSERT INTO `forum_questions` (`question`, `user_id` ,`grade_id`, `subject_id`, `status`,`created`) VALUES ('".$pre_text2."', '".$user->uid."', '".$form_state['values']['grade']."', '".$form_state['values']['subject']."','0',CURRENT_TIMESTAMP)";
	    if(db_query($forum_question_query))
	    {			
		    drupal_set_message('New Question has been posted Successfully');
		}
		else
		{
		    drupal_set_message('Error while posting a question , Please try again.');
		}
        $last = db_query("SELECT MAX(id) as last FROM forum_questions")->fetchObject();
        $file_attachment = db_query("SELECT file_name FROM forum_question_attach WHERE question_id = ".$last->last);
        $output = '';
        while($link = $file_attachment->fetchObject())
        {
            $output .= '<a href='.$link->file_name.' download>Click Here To Download</a><br>';
        }
        $message = array('0'=>t('Dear Moderator,<br><br>A question has been posted on forums and is waiting for the approval<br>
                                 Grade: '.grade($form_state['values']['grade']).'<br>
                                 Topic: '.subject($form_state['values']['subject']).'<br>
                                 Question: '.$form_state['values']['question_desc'].'<br>
                                 File Attachment : '.$output.'')); 
        $params = array(
            'body' => $message,
            'subject' => 'New Question Posted on Forum',
        );
        
        $admin_moderator_id = role_mail('admin moderator',$form_state['values']['grade'],$form_state['values']['subject']);
        $moderator_id = role_mail('moderator',$form_state['values']['grade'],$form_state['values']['subject']);
		if($moderator_id)
		{
        	$to = $admin_moderator_id.','.$moderator_id;
		}
		else
		{
			$to = $admin_moderator_id;
			
		}
        //$to = 'dineshsgaddi@gmail.com,manjiris@winjit.com';
        $language = user_preferred_language($user);
        drupal_mail('forum', 'askaquestion', $to, $language, $params, 'demo@demo.com', $send = TRUE);
		drupal_goto("forum/questions/".$form_state['values']['grade']."/".$form_state['values']['subject']."/");        
    }
	function forum()
	{
	    //return 1;
	}
	function _forum_question()
	{   
	   global $base_path;
	   global $user;
	   $usr = $user->uid;
	   $grades = arg(2);
	   $subjects = arg(3);
	   $output = '';
	   if (($grades)||($subjects))
	   {
		$student_query = db_query('SELECT count(*) as cnt FROM users_extend  WHERE uid = '.$usr.' AND grade = '.$grades.' AND FIND_IN_SET('.$subjects.',subject) > 0')->fetchObject();
	    if((in_array('admin moderator',$user->roles)) || (in_array('moderator',$user->roles)&&($student_query->cnt > 0)) || ((in_array('teacher',$user->roles))&&($student_query->cnt > 0)) || (in_array('administrator',$user->roles)))
	    {
		   $header = array(
							array('data' => 'Question', 'field' => 'question', 'sort' => 'desc'),
							array('data' => 'File', 'field' => 'id', 'sort' => 'asc'),
							array('data' => 'Posted By', 'field' => 'created', 'sort' => 'desc'),
							array('data' => 'Status'),
						  );
	    }
	    else
	    {
		   $header = array(
							array('data' => 'Question', 'field' => 'question', 'sort' => 'desc'),
							array('data' => 'File', 'field' => 'id', 'sort' => 'asc'),
							array('data' => 'Posted By', 'field' => 'created', 'sort' => 'desc'),
						  );
	    }
	   
	   if(!($grades)||!($subjects))
	   {
           $result = db_select('forum_questions','fq');
           $result->join('forum_grades','fg', 'fq.grade_id = fg.id');
           $result->join('forum_subjects','fs', 'fq.subject_id = fs.id');		   
		   if(!user_is_logged_in())
		   {
				$result->join('forum_question_reply','fqr', 'fqr.question_id = fq.id');
				$result->condition('fqr.status', 1, '=');
		   }
           $result->fields('fs', array('subject_name'));
           $result->fields('fg', array('grade_name'));
           $result->fields('fq', array('question','user_id','created','id'));
           $result->condition('fq.status', 1, '=')
				  ->execute()
                  ->fetchAll();
           $table_sort = $result->extend('TableSort')
                       ->orderByHeader($header);
           $pager = $table_sort->extend('PagerDefault')
                  ->orderBy('id', 'DESC')
                  ->limit(5);
           $result = $pager->execute();
	   }
	   else
	   {
	       
	       $res_grades = db_query('SELECT grade_name FROM forum_grades WHERE id ='.$grades)->fetchObject();
		   $res_topics = db_query('SELECT subject_name FROM forum_subjects WHERE id ='.$subjects)->fetchObject();
		   $output .= "<div class='font-100 custom_font content_down sub-title'>
  	                    Grade :<label class='font_black font-100'> ".$res_grades->grade_name." </label> &nbsp; &nbsp;
  	                    Topic :<label class='font_black font-100'> ".$res_topics->subject_name." </label> 
  	                  </div>";	     
	       $result = db_select('forum_questions','fq');
		   $result->distinct();
           $result->join('forum_grades','fg', 'fq.grade_id = fg.id');
           $result->join('forum_subjects','fs', 'fq.subject_id = fs.id');
		   if((!in_array('teacher',$user->roles))&&(!in_array('admin moderator',$user->roles))&&(!in_array('moderator',$user->roles))&&(!in_array('administrator',$user->roles)))
		   {
				$result->join('forum_question_reply','fqr', 'fq.id = fqr.question_id');
				$result->condition('fqr.status', 1, '=');
		   }
           $result->fields('fs', array('subject_name'));
           $result->fields('fg', array('grade_name'));
           $result->fields('fq', array('question','user_id','created','id'));
           $result->condition('fq.status', 1, '=')
                  ->condition('fg.id', $grades,'=')
                  ->condition('fs.id', $subjects,'=')
                  ->execute()
                  ->fetchAll();
           $table_sort = $result->extend('TableSort')
                       ->orderByHeader($header);
           $pager = $table_sort->extend('PagerDefault')
                  ->orderBy('created', 'DESC')
                  ->limit(5);
           $result = $pager->execute();
           
	   }
	   if($result->rowCount() > 0)
	   {
				while ($item = $result->fetch())
				{
					$query_user = user_load($item->user_id);
					$query_username = $query_user->name;
					$date_ago = ago($item->created);
					$comment_query = db_query("SELECT count(*) as cmt_count FROM forum_question_reply WHERE question_id='".$item->id."'")->fetchObject();
					if($comment_query->cmt_count > 0)
					{
						$comment_status = "<span>&#10004;</span>";
					}
					else
					{
						$comment_status = "<span><strong>pending</strong></span>";
					}
					$attacment_query = db_query('SELECT question_id, file_name FROM forum_question_attach WHERE question_id = '.$item->id);
					$link="-";
					while ($row_attachment = $attacment_query->fetchObject())
					{					
						if($row_attachment->file_name)
							$link="<a href='".base_path().$row_attachment->file_name."' target='_blank'>File</a>";
						else
							$link="-";
					}
					if((in_array('admin moderator',$user->roles)&&($student_query->cnt > 0)) || (in_array('moderator',$user->roles)) || ((in_array('teacher',$user->roles))&&($student_query->cnt > 0)) || (in_array('administrator',$user->roles)))
					{
					$row[] = array(l(truncate_chars($item->question,30),'forum/question-detail/'.$item->id.''),$link,$query_username.'<div class="algn-right small-font">'.$date_ago.'</div>',$comment_status);
					}
					else
					{
					$row[] = array(l(truncate_chars($item->question,30),'forum/question-detail/'.$item->id.''),$link,$query_username.'<div class="algn-right small-font">'.$date_ago.'</div>');
					}
				}
				$output .= theme('table',array('header' => $header,'rows' => $row));
				$output .= theme('pager');
				if((in_array('admin moderator',$user->roles)) || (in_array('moderator',$user->roles)&&($student_query->cnt > 0)) || (in_array('teacher',$user->roles)&&($student_query->cnt > 0)))
				{}
				else
				{
					$baseurl=base_path()."ask-a-question";
					$output .= "<div class='content_down clear empty_content'><a href='".$baseurl."' class='btn btn-default' >Ask a question</a></div>";
				}
		}
		else
		{
				if((in_array('admin moderator',$user->roles)) || (in_array('moderator',$user->roles)&&($student_query->cnt > 0)))
				{}
				else
				{
				$baseurl1=base_path()."forums";
				$baseurl=base_path()."ask-a-question";
				$output .= "<div class='sub-title clear'>No questions have been posted.</div><div class='col-12 content_down'><a href='".$baseurl1."'  class='btn btn-default' >Back</a> &nbsp;<a href='".$baseurl."' class='btn btn-default' >Ask a question</a></div>";
				}
		}
	   
	      
	   }
		else
		{
			drupal_goto("forums");
			//var_dump($grade);
			//var_dump($subjects);
			//die;
		}
	   return $output;
	}
    function _question()
    {
        global $user;
        global $base_url;
        $account = $user->uid;
        $question_id = arg(2);
        $output = '';
        //$comment = db_query('SELECT qr.reply_message, qr.id,qr.user_id,qr.created FROM forum_question_reply as qr WHERE qr.question_id = '.$question_id.' AND qr.status = 1');
		$grd_subj = db_query("SELECT grade_id,subject_id FROM forum_questions WHERE id=".$question_id)->fetchObject();
		$grades = $grd_subj->grade_id;
		$subjects = $grd_subj->subject_id;
		$student_query = db_query('SELECT count(*) as cnt FROM users_extend  WHERE uid = '.$account.' AND grade = '.$grades.' AND FIND_IN_SET('.$subjects.',subject) > 0')->fetchObject();
        if($question_id)
        {
            $question_detail = db_query('SELECT grds.grade_name, sub.subject_name, ques.question, ques.user_id, ques.created, ques.id FROM forum_grades as grds, forum_subjects as sub, forum_questions as ques WHERE ques.grade_id = grds.id and ques.subject_id = sub.id and ques.id = '.$question_id.'');		
		    $res_attachment = db_query('SELECT id,question_id,file_name from forum_question_attach where question_id='.$question_id.'');
            $question_reply = db_query('SELECT qr.reply_message, qr.id,qr.user_id,qr.created FROM forum_question_reply as qr WHERE qr.question_id = '.$question_id.' AND qr.status = 1');
            $output .= "<div>";
  	        while($rows = $question_detail->fetchObject()){
			    $grade_name=$rows->grade_name;
			    $subject_name=$rows->subject_name;
		        $output .= "<div class='col-12 img_bg border_blue'>
		                        <div class='img_bg '>
		                            <div class='space'>
  	                                    <div class='font-100 custom_font2 content_down '>
  	                                    Grade :<label class='font_black font-300'> ".$rows->grade_name." </label> &nbsp; &nbsp;              
  	                                    Topic :<label class='font_black font-300'> ".$rows->subject_name." </label>
  	                                </div>
  	                            </div>";
  	    
                $date_ago = ago($rows->created);
	            $temp_user = user_load($rows->user_id);
                $temp_username = $temp_user->name;
                $output .= "<div class='space content-font'>
                                <div>
                        	        <span>Posted By:</span><strong><span>".$temp_username."</span></strong>
                                </div>
                                <div>
                        	        <span>Date - </span><strong><span>".date('F jS Y', strtotime($rows->created))."</span></strong>
                                </div>";
		 			
                $output .= "<div class='content-font'>
                                <div>Question:</div>
                                <h5 class='font-300 font-black'><p style='text-indent: auto;'><strong>".nl2br($rows->question)."</strong></p></h5>";
				        		while($rows_attach = $res_attachment->fetchObject()){
		 		        			if($rows_attach->file_name)	
		 		        				$output .="<a href='".base_path().$rows_attach->file_name."' target='_blank' class='font_blue'>See file attached</a><br />";
				            		}// '../../".$rows_attach->file_name."'
			    $output .="</div></div>";	
            }
        $question_detail1 = db_query('SELECT ques.grade_id, ques.subject_id, ques.user_id, ques.status FROM forum_questions as ques WHERE ques.id = '.$question_id.'');
        $row = $question_detail1->fetchObject();
        $teacher_query = db_query("SELECT * FROM users_extend WHERE uid = '".$user->uid."' AND grade = '".$row->grade_id."' AND FIND_IN_SET('".$row->subject_id."', subject)");
        if(((in_array('teacher', $user->roles))&&($teacher_query->rowCount() > 0))||(in_array('administrator', $user->roles)))
        {
            $form_question = drupal_get_form("question_reply_form", $question_id);
        }
        else if((!user_is_logged_in())||(in_array('authenticated user', $user->roles)))
        {
		}
        else
        {
            drupal_set_message('You dont have access-rights to view this page','warning');
        }		
        if($question_reply->rowCount() > 0)
	    {
            while($reply = $question_reply->fetchObject())
            {
				$reply_temp_user = user_load($reply->user_id);
				$reply_user = $reply_temp_user->name;
                $output .= "<div class='space content-font'>
                            <div>
                            <span>Replied By:</span><strong><span>".$reply_user."</span></strong>
                            </div>
                            <div>
                            <span>Date - </span><strong><span>".date('F jS Y', strtotime($reply->created))."</span></strong>
                            </div>";
                $output .= "<div id='form_{$reply->id}' class='form-wrapper'>";
                $output .= "<div>Answer:</div><h5 class='font-300 font-black'><strong>".nl2br($reply->reply_message)."</strong></h5>";
                $output .= "</div>";
                $output .= "</div><br>";
            }
			
        }
        else if(((in_array('admin moderator',$user->roles))||((in_array('moderator',$user->roles)&&($student_query->cnt > 0))||(in_array('administrator',$user->roles))))&&($row->status == '0'))
        {
           $output.="<div class='col-12 content_down'><a href='".$base_url."/forum/question/approval/".$question_id."/1' class='btn btn-default'>Accept</a> <a href='".$base_url."/forum/question/approval/".$question_id."/-1' class='btn btn-default'>Reject</a><br /><br /></div>"; 
        }
        else
        {
                $output .= "<div class='space'>";
                $output .= drupal_render($form_question);
		        $prev_url=base_path()."forum/questions/".$row->grade_id."/".$row->subject_id;
		        $output .= "<a href='".$prev_url."'  class='btn btn-default content_down' > Go to ".$grade_name." &amp; ".$subject_name."</a><br><br>";
                $output .= "</div>";
        }
        $output .= "</div><br></div>";
        }
        else
        {
            $output.="No Questions have been posted!"; 
        }
        return $output;
    }
    function question_reply_form($form, &$form_state, $question_id) {
	global $user;
	
	
        $form = array();
        $form["#submit"] = array(
            'question_reply_form_submit',
        );
        $form["content"] = array(
            '#type' => 'textarea',
            '#wysiwyg' => True,
            '#resizable' => FALSE,
            '#required' => TRUE,
            '#attributes' => array(
                    'class' => array('question-reply')
            ),
        );
        $form["hidden"] = array(
            '#type' => 'hidden',
            '#value' => $question_id
        );
        $form["submit"] = array(
            '#type' => 'submit',
            '#value' => "Send Email"
        );
        return $form;
	
    }
    function question_reply_form_submit($form, &$form_state)
    {
        global $user;
        $query = "INSERT INTO `forum_question_reply` (`question_id`, `user_id` ,`reply_message`, `status`,`created`) VALUES ('".$form_state["values"]["hidden"]."', '".$user->uid."',  '".$form_state["values"]["content"]."', 0,CURRENT_TIMESTAMP)";        
        if(db_query($query)){
			drupal_set_message('Your reply has been sent successfully, waiting for Moderators Approval');
		}else {
			drupal_set_message('Error while posting a commment , Please try again.');
		}
		$stu_query = db_query("SELECT user_id,grade_id,subject_id,question FROM forum_questions WHERE id = ".$form_state["values"]["hidden"])->fetchObject();
		$message = array('0'=>t('Dear Moderator,<br><br>Answer has been given for question and is waiting for your approval<br>
                                         Grade: '.grade($stu_query->grade_id).'<br>
                                         Topic: '.subject($stu_query->subject_id).'<br>
                                         Question: '.$stu_query->question.' <br>
										 Answer: '.$form_state['values']['content'].''));
        $params = array(
            'body' => $message,
            'subject' => 'Answer is waiting for your approval',
        );
        $admin_moderator_id = role_mail('admin moderator',$stu_query->grade_id,$stu_query->subject_id);
        $moderator_id = role_mail('moderator',$stu_query->grade_id,$stu_query->subject_id);
        $to = $admin_moderator_id.','.$moderator_id;
        $language = user_preferred_language($user);
        drupal_mail('forum', 'replyaquestion', $to, $language, $params, 'demo@demo.com', $send = TRUE);
       // drupal_goto('forums');
	   drupal_goto('forum/questions/'.$stu_query->grade_id.'/'.$stu_query->subject_id.'/');
    }
?>

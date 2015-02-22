<?php

    /*------------------------------------
        Form for student asking a question
    --------------------------------------*/
	function ask_a_question_form($form_state)
	{
	    global $user;
			
		//if($user->uid){
		/*if(in_array('administrator',$user->roles) || in_array('authenticated user',$user->roles)){
		if(!in_array('anonymous user',$user->roles))*/
		if($user->uid){
	    $result_grade = db_query('SELECT id,grade_name FROM forum_grades') ->fetchAll();
        $grd_nm = $result_grade[0]->grade_name;
        $grd_id = $result_grade[0]->id;
        $option[''] = '----Select----';        
        foreach ($result_grade as $node) {
            $option[$node->id] = $node->grade_name;
        }
        $form['grade'] = array(
            '#title' => t('Grade'),
            '#type' => 'select',
            '#options' => $option,
            '#attributes' => array('class' => array('grade')),
            '#validated' => TRUE,
            '#required' => TRUE,
        );
        $form['subject'] = array(
            '#title' => t('Topic'),
            '#type' => 'select',
            '#options' => array(''=> '-- Select --'),
            '#attributes' => array('class' => array('subject')),
            '#validated' => TRUE,
            '#required' => TRUE,
        );
        $form['file'] = array(
            '#type' => 'file',
            '#name' => 'files[]',
            '#title' => t('Attach Multiple Files'),
            '#description' => t('You are allow to upload jpg, jpeg, png and gif, 10MB Max Size'),
            '#attributes' => array('multiple' => 'multiple'),
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
            drupal_set_message('You need to login to post a question <a href="node/53">Login</a>');
        }
	}

    function ask_a_question_form_validate($from, $form_state)
    {
        if($form_state['values']['grade'] == '') { 
            form_set_error('grade', t('Please select a Grade.')); 
        }
        if($form_state['values']['subject'] == '') { 
            form_set_error('grade', t('Please select a Topic.')); 
        } 
    }
	
	function ask_a_question_form_submit($form, &$form_state)
	{
		
	    global $user;
	    $pre_text = escape($form_state['values']['question_desc']);
	    /*$query = "INSERT INTO `forum_questions` (`question`, `user_id` ,`grade_id`, `subject_id`, `status`,`created`) VALUES ('".$pre_text."', '".$user->uid."',  '".$form_state['values']['grade']."', '".$form_state['values']['subject']."','0',CURRENT_TIMESTAMP)";*/
		
		//$pre_text2 = htmlspecialchars($form_state['values']['question_desc'],ENT_QUOTES);
		$pre_text2 = htmlentities($form_state['values']['question_desc'],ENT_QUOTES);
		
		$query = "INSERT INTO `forum_questions` (`question`, `user_id` ,`grade_id`, `subject_id`, `status`,`created`) VALUES ('".$pre_text2."', '".$user->uid."',  '".$form_state['values']['grade']."', '".$form_state['values']['subject']."','0',CURRENT_TIMESTAMP)";
	    if(db_query($query)){
			drupal_set_message('New Question has been posted Successfully');
		}else {
			drupal_set_message('Error while posting a question , Please try again.');
		}
	    $last = db_query("SELECT MAX(id) as last FROM forum_questions");
	    $last = $last->fetchObject();
	    $last = $last->last;

	    /*---------------------------------------------------------------------
	     Code for attaching multiple files along with the question on the forum
	    ----------------------------------------------------------------------*/
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
                $name = $_FILES['files']['name'][$key];
                $tmp = explode(".", $name);
                $extension=end($tmp);
                $name="Attachment-".$key.".".$extension;
                //if (($_FILES['files']['size'][$key] < 20000) && in_array($extension, $allowedExts))
				
                if (in_array($extension, $allowedExts)){
                    $moved = move_uploaded_file($tmp_name, $directory.'/'.$name);
                }
                else{
                        drupal_set_message(t($directory.'/'.$name.'Some file cannot be uploaded on the forum. Please contact the administrator of this site.'), 'error');
                }
                $path_name = $directory.'/'.$name;
                $query = db_query("INSERT INTO `forum_question_attach` (`question_id`, `file_name` ,`file_type`) VALUES ('".$last."', '".$path_name."','".$_FILES['files']['type'][$key]."')");
            }
        }
        $header = "MIME-Version: 1.0\r\n";
        
        $last = db_query("SELECT MAX(id) as last FROM forum_questions")->fetchObject();
        
        $file_attachment = db_query("SELECT file_name FROM forum_question_attach WHERE question_id = ".$last->last);
        //foreach $link as $file_attachment
        $output = '';
        while($link = $file_attachment->fetchObject())
        {
            $output .= '<a href='.$link->file_name.' download>Click Here To Download</a><br>';
        }
        $message = array('0'=>t('Dear Moderator,<br><br>A question has been posted on forums and is waiting for the approval<br>
                                 Grade: '.grade($form_state['values']['grade']).'<br>
                                 Topic: '.subject($form_state['values']['subject']).'<br>
                                 Question: '.$form_state['values']['question_desc'].'<br>
                                 File Attachment : '.$output.'')); // Body of your email here.
        /*$attachment = array(
            'filecontent' => $directory,
            'filename' => $name,// You can change the name and extension of the file         you need to send.
            'filemime' => $filemime, 
        );*/
        
        $params = array(
            'body' => $message,
            'subject' => 'New Question Posted on Forum',
            'headers'=> array('Content-Type' => 'text/html'),
            //'attachment' => $attachment,
        );
        
        $moderator_id = role_mail('moderator',$form_state['values']['grade'],$form_state['values']['subject']);
        $admin_moderator_id = role_mail('admin moderator',$form_state['values']['grade'],$form_state['values']['subject']);
        $to = $moderator_id.",".$admin_moderator_id;
        $language = user_preferred_language($user);
        drupal_mail('forum', 'replyaquestion', $to, $language, $params, 'demo@demo.com', $send = TRUE);
	}
	
	function forum()
	{
	    //return 1;
	}
	
	/*------------------------------------------
	    Listing all the questions on the forum
	-------------------------------------------*/
	function _forum_question()
	{   
	   global $base_path;
	   global $user;
	   $grades = arg(2);
	   $subjects = arg(3);
	   $output = '';
	   
	   $header = array(
                        array('data' => 'Grade', 'field' => 'subject_name', 'sort' => 'desc'),
                        array('data' => 'Topic', 'field' => 'grade_name', 'sort' => 'desc'),
                        array('data' => 'Question', 'field' => 'question', 'sort' => 'desc'),
                        array('data' => 'Posted By', 'field' => 'created', 'sort' => 'desc'),
                        );
	   
	   if (!($grades)||!($subjects))
	   {
           $result = db_select('forum_questions','fq');
           $result->distinct();
           $result->join('forum_grades','fg', 'fq.grade_id = fg.id');
           $result->join('forum_subjects','fs', 'fq.subject_id = fs.id');
		   //if(!user_is_logged_in())
		   //{
				$result->join('forum_question_reply','fqr', 'fqr.question_id = fq.id');
		   //}
           $result->fields('fs', array('subject_name'));
           $result->fields('fg', array('grade_name'));
           $result->fields('fq', array('question','user_id','created','id'));
           $result->condition('fqr.status', 1, '=');
           $result->condition('fq.status', 1, '=')
				  ->condition('fqr.status', 1, '=')
				  ->execute()
                  ->fetchAll();
           $table_sort = $result->extend('TableSort')
                       ->orderByHeader($header);
           $pager = $table_sort->extend('PagerDefault')
                  ->orderBy('created', 'DESC')
                  ->limit(5);
           $result = $pager->execute();
	   }
	   else
	   {
	       $result = db_select('forum_questions','fq');
		   $result->distinct();
           $result->join('forum_grades','fg', 'fq.grade_id = fg.id');
           $result->join('forum_subjects','fs', 'fq.subject_id = fs.id');
		   //if(!user_is_logged_in())
		   //{
				$result->join('forum_question_reply','fqr', 'fq.id = fqr.question_id');
		   //}
           $result->fields('fs', array('subject_name'));
           $result->fields('fg', array('grade_name'));
           $result->fields('fq', array('question','user_id','created','id'));
           $result->condition('fq.status', 1, '=')
				  ->condition('fqr.status', 1, '=')
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
				//$output .= html_entity_decode($item->question)."#";
				//$abc= nl2br($item->question);
				//$text= str_replace('<br />', PHP_EOL,$abc);
                $row[] = array($item->grade_name,$item->subject_name,l(".$item->question.",'forum/question-detail/'.$item->id.''),$query_username.'<div class="algn-right small-font">'.$date_ago.'</div>');
            }
            $output .= theme('table',array('header' => $header,'rows' => $row));
            $output .= theme('pager');
			$baseurl=base_path()."ask-a-question";
			/*$output .= "<div class='content_down clear'>Not found solution for your question... <a href='../../../../ask-a-question' class='btn btn-default' >Ask a question</a></div>";*/
			$output .= "<div class='content_down clear empty_content'><a href='".$baseurl."' class='btn btn-default' >Ask a question</a></div>";
       }
       else
	   {
			$baseurl1=base_path()."forums";
			$baseurl=base_path()."ask-a-question";
	    	$output .= "<div class='sub-title clear'>No questions have been posted.<a href='".$baseurl1."'>Go Back to Forum</a></div><br /><a href='".$baseurl."' class='btn btn-default' >Ask a question</a>";
	   }
	   return $output;
	}

    /*------------------------------------------------------
        This page is to view single question and there reply
    -------------------------------------------------------*/
    
    function _question()
    {
        global $user;
        $account = $user;
        $question_id = arg(2);
        $output = '';
        $question_detail = db_query('SELECT grds.grade_name, sub.subject_name, ques.question, ques.user_id, ques.created, ques.id FROM forum_grades as grds, forum_subjects as sub, forum_questions as ques WHERE ques.grade_id = grds.id and ques.subject_id = sub.id and ques.id = '.$question_id.'');
        $question_reply = db_query('SELECT qr.reply_message, qr.id,qr.user_id,qr.created FROM forum_question_reply as qr WHERE qr.question_id = '.$question_id.' AND qr.status = 1');
        $output .= "<div>";
  	    while($rows = $question_detail->fetchObject()){
		 $output .= "<div class='col-12'><div class='img_bg border_blue'><div class='space'>
  	                <div class='font-100 custom_font2 content_down ' style='display:none;'>
  	                    Grade :<label class='font_black font-300'> ".$rows->grade_name." </label> &nbsp; &nbsp;               
  	                    Topic :<label class='font_black font-300'> ".$rows->subject_name." </label> 
  	                </div></div>";
  	    
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
                        <h5 class='font-300 font-black'><p style='text-indent: auto;'><strong>".nl2br($rows->question)."</strong></p></h5></div></div>";
        }
       
        $question_detail1 = db_query('SELECT ques.grade_id, ques.subject_id, ques.user_id FROM forum_questions as ques WHERE ques.id = '.$question_id.'');
        $row = $question_detail1->fetchObject();
        $teacher_query = db_query("SELECT * FROM users_extend WHERE uid = '".$user->uid."' AND grade = '".$row->grade_id."' AND FIND_IN_SET('".$row->subject_id."', subject)");
        if($teacher_query->rowCount() > 0)
        {
            $form_question = drupal_get_form("question_reply_form", $question_id);
        }
        else if($account->uid <= '0')
        {
        }
        else
        {
            drupal_set_message('You dont have access-rights to view this page');
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
        $output .= "<div class='space'>";
        $output .= drupal_render($form_question);
        $output .= "</div></div></div>";
        return $output;
    }
    
    /*----------------------------------------------------
        This form is for replying the questions on forum
    -----------------------------------------------------*/
    
    function question_reply_form($form, &$form_state, $question_id) {
        $form = array();
        $form["#submit"] = array(
            'question_reply_form_submit',
        );

        $form["content"] = array(
            '#type' => 'textarea',
            '#wysiwyg' => True,
            '#resizable' => FALSE,
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
            '#value' => "Send Mail"
        );
        return $form;
    }
    
    function question_reply_form_submit($form, &$form_state)
    {
        global $user;
        $query = "INSERT INTO `forum_question_reply` (`question_id`, `user_id` ,`reply_message`, `status`,`created`) VALUES ('".$form_state["values"]["hidden"]."', '".$user->uid."',  '".$form_state["values"]["content"]."', 0,CURRENT_TIMESTAMP)";        
        if(db_query($query)){
			drupal_set_message('Your Comment is waiting for Moderators Approval');
		}else {
			drupal_set_message('Error while posting a commment , Please try again.');
		}
		$message = array('0'=>t('Dear User,<br>A question has been replied')); // Body of your email here.
        $params = array(
            'body' => $message,
            'subject' => 'Answer For Your Question Posted on Forum',
            'headers'=>'simple',
        );
        
        
        //$to = "dineshsgaddi@gmail.com";
		$to = "manjiris@winjit.com";
        $language = user_preferred_language($user);
        drupal_mail('forum', 'replyaquestion', $to, $language, $params, 'demo@demo.com', $send = TRUE);
    }
    
    function escape($value) {
    $return = '';
    for($i = 0; $i < strlen($value); ++$i) {
        $char = $value[$i];
        $ord = ord($char);
        if($char !== "'" && $char !== "\"" && $char !== '\\' && $ord >= 32 && $ord <= 126)
            $return .= $char;
        else
            $return .= '\\x' . dechex($ord);
    }
    return $return;
    }
?>

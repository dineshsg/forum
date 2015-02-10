<?php
    /*-------------------------------------------
        Questions waiting for Moderators Approval
    ----------------------------------------------*/
    function question_waiting_for_approval()
    {
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
                    drupal_set_message('New Question has been posted on Forum');
                }else {
                    drupal_set_message('Error !!!!.Please try again.');
                }
                drupal_goto('forum/question/approval');
            }
            else
            {
                /*----------------------------------------------
                    Question which are rejected by the Moderator
                ------------------------------------------------*/
                $query_rej = "UPDATE `forum_questions` SET `status`='".$q_status."', `updated` = CURRENT_TIMESTAMP WHERE id = '".$q_id."'";            
                if(db_query($query_rej)){
                    drupal_set_message('New Question has been Rejected');
                }else {
                    drupal_set_message('Error !!!!.Please try again.');
                }
                drupal_goto('forum/question/approval');
            }
            
        }
        else
        {
                /*------------------------------------------------------------
                    List of questions which are waiting for moderators approval
                --------------------------------------------------------------*/
        $output = '';
        $row = array();
        $header = array(
                        array('data' => 'Subject Name', 'field' => 'subject_name', 'sort' => 'asc'),
                        array('data' => 'Grade Name', 'field' => 'grade_name', 'sort' => 'asc'),
                        array('data' => 'Question', 'field' => 'question', 'sort' => 'asc'),
                        array('data' => 'Posted By', 'field' => 'user_id', 'sort' => 'asc'),
                        array('data' =>'Action', 'field' => '', 'sort' => 'asc')
                        );
        $result = db_select('forum_questions','fq');
        $result->join('forum_grades','fg', 'fq.grade_id = fg.id');
        $result->join('forum_subjects','fs', 'fq.subject_id = fs.id');
        $result->fields('fs', array('subject_name'));
        $result->fields('fg', array('grade_name'));
        $result->fields('fq', array('question','user_id','created','id'));
        $result->execute()
               ->fetchAll();
        $table_sort = $result->extend('TableSort')
                    ->orderByHeader($header);
        $pager = $table_sort->extend('PagerDefault')
               ->orderBy('created', 'ASC')
               ->limit(5);
        $result = $pager->execute();
        if(!$result->rowCount())
        {
            $output .= "No Questions for approval!!";
        }
        else
        {
            while ($item = $result->fetch())
            {
                $user = user_load($item->user_id);
                $username = $user->name;
                $date_ago = ago($item->created);
                $row[] = array($item->grade_name,$item->subject_name,$item->question,$username.'<div class="algn-right small-font">'.$date_ago.'</div>',l('Accept','approval/'.$item->id.'/1').' | '.l('Reject','approval/'.$item->id.'/-1'));
            }
            $output .= theme('table',array('header' => $header,'rows' => $row));
            $output .= theme('pager');
        }
	    return $output;
	    }
    }
    
?>

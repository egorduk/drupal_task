<?php
class ProductController extends EntityAPIController {
    public function create(array $values = array()) {
        global $product, $image, $type;
        $values += array(
            'id' => $product->id,
            'name' => $product->name,
            'info' => $product->info,
            'created_at' => $product->created_at,
            'updated_at' => $product->updated_at,
            'image_id' => $image->fid,
            'type_id' => $type->id,
        );
        return parent::create($values);
    }

    public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
        $build = parent::buildContent($entity, $view_mode, $langcode, $content);
        global $user;
        $links = array();
        if ($view_mode == 'teaser') {
            $links['smessage-readmore'] = array(
                'title' => t('Read more'),
                'href' => $entity->real_path(),
                'html' => TRUE,
            );
        }
        // Ссылку "Ответить" выводим только если пользователь - получатель и это не страница ответа.
        if ($user->uid == $entity->recipient_uid &&
            (!isset($entity->op) || 'reply' != $entity->op)) {
            $links['smessage-reply'] = array(
                'title' => t('Reply'),
                'href' => $entity->real_path() . '/reply',
                'html' => TRUE,
            );
        }
        // Добавляем блок ссылок только если они есть (хотя бы одна).
        if (!empty($links)) {
            $build['links'] = array(
                '#theme' => 'links__smessage',
                '#pre_render' => array('drupal_pre_render_links'),
                '#attributes' => array('class' => array('links', 'inline')),
            );
            $build['links']['smessage'] = array(
                '#theme' => 'links__smessage__smessage',
                '#links' => $links,
                '#attributes' => array('class' => array('links', 'inline')),
            );
        }
        return $build;
    }

    public function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL) {
        // Основную работу делают за нас.
        $view = parent::view($entities, $view_mode, $langcode, $page);
        if (!empty($view)) {
            // Мы просто поставим свою темизацию, соберём id сообщений,
            // у которых надо обновить время прочтения и, попутно, покажем
            // для таких сообщений, что они прочитаны, прямо в текущем выводе.
            global $user;
            $readed_ids = array();
            $read_time = REQUEST_TIME;
                foreach ($view[$this->entityType] as &$entity) {
                $smessage = &$entity['#entity'];
                if ($smessage->recipient_uid == $user->uid && 'full' == $view_mode) {
                    $readed_ids[] = $smessage->smessage_id;
                    // Показываем, что сообщение прочитано, для текущего вывода.
                    $smessage->read_time = $read_time;
                }
                // Наша темизация.
                $entity['#theme'] = $this->entityType;
            }
            unset($entity);
            // Сохраняем для выбранных сообщений время прочтения.
            if ($readed_ids) {
                db_update('product')
                    ->fields(array('read_time' => $read_time))
                    ->condition('id', $readed_ids)
                    ->execute();
            }
        }
        return $view;
    }
}
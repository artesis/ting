<?php
/**
 * @file
 * Search items with images.
 *
 * Helper functions used in:
 *   ding_item_list
 *   ding_item_viewer
 *   ting_search_carousel.
 */

/**
 * Get ting objects with images.
 *
 * @param string $query
 *   Search query string.
 * @param int $count
 *   Number of objects to search.
 * @param string $sort
 *   Sort search results.
 * @param array $image_styles
 *   Array of image style names for covers.
 *
 * @return array
 *   TingObjects.
 */
function ting_get_objects_with_images($query = '', $count = 0, $sort = '', $image_styles = array('large')) {
  module_load_include('pages.inc', 'ting_covers');

  $objects = array();
  $request = '(' . $query . ')';
  $page = 1;

  $covers_service = TRUE;

  if (!empty($query) && !empty($count)) {
    // Search with portions of 10 items.
    $result = _ting_search($request, $page++, 10, $sort);
    while (count($result)) {

      // Check images.
      $images = array();
      if ($covers_service) {
        foreach ($result as $search_item) {
          // Build input for ting cover.
          foreach ($image_styles as $image_style) {
            $images[] = $search_item->getLocalId() . ':' . $image_style;
          }
        }
        try {
          $images = _ting_covers_objects($images, TRUE);
        }
        catch (Exception $e) {
          $covers_service = FALSE;
        }
      }

      // No images for for current result set, so get path for default image.
      if (empty($images) && $covers_service) {
        $images[$search_item->getLocalId() . ':' . $image_style] =
          '/' . drupal_get_path('module', 'ding_item_list') . '/images/no-image-minil.png';
      }

      $image_style = reset($image_styles);
      foreach ($result as $search_item) {
        // Check if this item has an image and the service is available.
        $image_style = reset($image_styles);
        $image_index = $search_item->getLocalId() . ':' . $image_style;
        if (empty($images[$image_index]) && $covers_service) {
          // No image, skip to another item.
          continue;
        }
        elseif(!$covers_service) {
          $images[$image_index] = NULL;
        }

        foreach ($image_styles as $image_style) {
          $image_index = $search_item->getLocalId() . ':' . $image_style;
          $search_item->setImage($images[$image_index], $image_style);
        }

        $objects[$search_item->getLocalId()] = $search_item;

        // We have needed amount of items.
        if (count($objects) == $count) {
          break(2); // Break for & while.
        }
      } // End for (building items).

      // Next portion of results.
      $result = _ting_search($request, $page++, 10, $sort);
    } // End while.

    // Make an array of local id's, fetchable by voxb.
    // Take only items with ISBN code.
    /*
    if (module_exists('ding_voxb')) {
      $local_ids = array();
      foreach ($objects as $local_id => $v) {
        $isbn = $v->isbn;
        if (!empty($isbn)) {
          $local_ids[$local_id] = $isbn[0];
        }
      }

      // Fetch details from voxb
      $voxb_details = new VoxbItems();
      $voxb_details->addReviewHandler('review', new VoxbReviews());
      $response = $voxb_details->fetchByISBN($local_ids);

      // Assign voxb details to items
      if ($response && $voxb_details->getCount() > 0) {
        foreach ($local_ids as $local_id => $isbn) {
          $detail = $voxb_details->getItem($isbn);
          if ($detail) {
            $objects[$local_id]->rating = (int) round($detail->getRating() / 20);
            $objects[$local_id]->rating_count = $detail->getRatingCount();
            $objects[$local_id]->comment_count = $detail->getReviews('review')
              ->getCount();
            $objects[$local_id]->has_rating = TRUE;
          }
        }
      }
    } // End VoxB.
    */
  } // End if.

  return $objects;
}

/**
 * Perform ting search.
 *
 * @param $query
 *   Searched query string.
 * @param $page
 *   Requested results page.
 * @param $records
 *   Number of records to fetch.
 * @return
 *   Ting search result object.
 */
function _ting_search($query = '', $page = 1, $records = 10, $sort = '') {
  module_load_include('client.inc', 'ting');

  if (!empty($query)) {
    $query = '(' . $query . ')';

    $options = array(
      'sort' => $sort,
      'skip_relations' => TRUE,
    );

    $search_result = ting_do_search($query, $page, $records, $options);

    return $search_result;
  }

  return FALSE;
}

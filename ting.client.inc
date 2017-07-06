<?php
/**
 * @file
 * Wrapper functions for Ting client.
 */
function ting_get_object_request($object_id) {
  $request = ting_get_request_factory('object');

  $request->setId($object_id);
  $request->setAllRelations(TRUE);
  $request->setRelationData('full');

  $result = ting_execute_cache($request);
  if (empty($result)) {
    return NULL;
  }

  $object = $result->getObject();

  $items = entity_load('ting_object', array(), array('ding_entity_id' => array($object->getObjectId())));

  return current($items);
}

/**
 * Get an ting object or collection.
 *
 * Returns the search response for the given id. This will cache the
 * result, and any sub-objects, so fetching objects from a recently
 * fetched collection won't trigger another backend request.
 *
 * @param string $object_id
 *   The id to fetch.
 * @param bool $collection
 *   Whether to return a collection, if possible, or an object.
 *
 * @todo Should use getObject, but the ting-client lib doesn't implement that.
 */
function ting_get_object($object_id, $collection = FALSE) {
  if (empty($object_id)) {
    return FALSE;
  }

  // Check the cache first.
  $object = ting_cache_get($object_id, $collection);
  if (!$object) {
    // Put a negative reply in the cache. It will be overwritten by the
    // object, or ensure that we won't try to fetch this id again.
    ting_cache_set($object_id, NULL);

    if (!$object) {
      $request = ting_get_request_factory('search');
      $request->setQuery('rec.id=' . $object_id);

      $result = ting_execute_cache($request);
      if (empty($result)) {
        return NULL;
      }

      foreach ($result as $res) {
        $object_id = $res->getObjectId();
        if (!empty($object_id)) {
          break;
        }
      }

      $request = ting_get_request_factory('object');
      $request->setId($object_id);
      $result = ting_execute_cache($request);
      if (empty($result)) {
        return NULL;
      }
      $object = $result->getObject();

      // Update data if it loaded from cache.
      if (is_a($result, 'TingObjectResult')) {
        $obj = $result->getObject();
        $object->setData($obj->getData());
      }
    }
  }

  // If not asking for a collection, and the object is, return the
  // sub-object with the same id.
  if (!$collection && isset($object->objects)) {
    foreach ($object->objects as $sub_object) {
      if ($sub_object->id == $object_id) {
        return $sub_object;
      }
    }
    // No sub-object had the same id. Somethings broken.

    return NULL;
  }
  return $object;
}

/**
 * Get a bunch of objects in one request.
 *
 * @todo Should use getObject when getObject supports getting multiple.
 */
function ting_get_objects($ids) {
  $objects = array();

  // Prefill from cache.
  foreach ($ids as $id) {
    $objects[$id] = ting_cache_get($id);
  }

  $query = array();
  $keys = array_keys($objects);
  foreach ($keys as $id) {
    if (empty($objects[$id])) {
      $object = ting_get_object($id);
      $objects[$id] = $object;
    }
  }

  return $objects;
}

/**
 * Performs a search agains the well
 *
 * @param string $query
 *    The search query
 * @param int $page
 *    The page number to retrieve search results for
 * @param int $results_per_page
 *    The number of results to include per page
 * @param array $options
 *    Options to pass to the search. Possible options are:
 *    - facets: Array of facet names for which to return results. Default: facet.subject, facet.creator, facet.type, facet.date, facet.language
 *    - numFacets: The number of terms to include with each facet. Default: 10
 *    - enrich: Whether to include additional information and cover images with each object. Default: false
 *    - sort: The key to sort the results by. Default: "" (corresponds to relevance). The possible values are defined by the sortType type in the XSD.
 *    - rank: The ranking type, as defined in the XSD.
 *    - supportingTypes: Whether to include supporting types such as reviews. Default: false
 *    - reply_only: Don't change the result objects to TingCollection objects.
 * @return TingClientSearchResult
 *    The search result
 */
function ting_do_search($query, $page = 1, $results_per_page = 10, $options = array()) {
  /* @var $request TingSearchRequest */
  $request = ting_get_request_factory('search');

  $request->setQuery($query);
  $request->setLimits($results_per_page * ($page - 1) + 1, $results_per_page);

  // Set facets.
  if (!isset($options['facets'])) {
    // Populate facets with configured facets in ding_facetbrowser.
    if (module_exists('ding_facetbrowser')) {
      $options['facets'] = array();
      foreach (variable_get('ding_facetbrowser_facets', array()) as $facet) {
        $options['facets'][] = $facet['name'];
      }
    }     // Default facets.
    else {
      $options['facets'] = array(
        'facet.subject',
        'facet.creator',
        'facet.type',
        'facet.category',
        'facet.language',
        'facet.date',
        'facet.acSource'
      );
    }
  }
  $request->setFacets($options['facets']);

  // Set number of terms in facets.
  if (!empty($options['facets'])) {
    $terms = 10;
    if (!empty($options['numFacets'])) {
      $terms = $options['numFacets'];
    }
    $request->setTermsInFacets($terms);
  }

  if (!empty($options['sort'])) {
    $request->setSort($options['sort']);
  }

  // Apply custom ranking if enabled.
  $ting_ranking_fields = variable_get('ting_ranking_fields', array());
  if (variable_get('ting_ranking_custom', FALSE) && !empty($ting_ranking_fields) && !isset($options['sort'])) {
    $fields = array();
    foreach ($ting_ranking_fields as $field) {
      $fields[] = array(
        'fieldName' => $field['field_name'],
        'fieldType' => $field['field_type'],
        'weight' => $field['weight']
      );
    }

    if (!empty($fields)) {
      // Add the default anyIndex boosts.
      $fields[] = array(
        'fieldName' => 'term.default',
        'fieldType' => 'phrase',
        'weight' => 2,
      );

      $fields[] = array(
        'fieldName' => 'term.default',
        'fieldType' => 'word',
        'weight' => 1,
      );

      $request->setUserDefinedRanking(
        array(
          'tieValue' => 0.1,
          'rankField' => $fields
        )
      );
    }
  }   // Otherwise, use the ranking setting.
  else {
    $request->setRank((isset($options['rank']) && $options['rank']) ? $options['rank'] : 'rank_general');
  }

  // Apply custom boosts if any.
  $boosts = variable_get('ting_boost_fields', array());

  if ($boosts) {
    $uboosts = array();
    foreach ($boosts as $boost_field) {
      $uboosts[] = array(
        'fieldName' => $boost_field['field_name'],
        'fieldValue' => $boost_field['field_value'],
        'weight' => $boost_field['weight']
      );
    }
    $request->setParameter('userDefinedBoost', $uboosts);
  }

  if (!empty($options['skip_relations'])) {
    $request->skipRelations();
  }

  $search_result = ting_execute_cache($request);

  // Replace collections with proper TingCollection objects.
  if ($search_result) {
    $ids = array();
    foreach ($search_result as $object) {
      $ids[] = $object->getObjectId();
    }

    if (empty($options['reply_only'])) {
      $items = entity_load('ting_object', array(), array('ding_entity_id' => $ids));
      $search_result->setItems($items);
    }
  }

  return $search_result;
}

/**
 * Calls ting_execute() and caches the result.
 *
 * Executes the request and caches sub-objects.
 *
 * @param object $request
 *   The request.
 */
function ting_execute_cache($request) {
  $calls = &drupal_static(__FUNCTION__);
  if (!isset($calls)) {
    $calls = array();
  }
  $calls[] = $request->getParameters();
  $reply = ting_execute($request);

  if (empty($reply)) {
    return $reply;
  }

  // Cache objects.
  foreach ($reply as $object) {
    ting_cache_set($object->getObjectId(), $object);

    // Cache relation objects.
    $relations = $object->getRelations();
    foreach ($relations as $relation) {
      if (!empty($relation['object'])) {
        ting_cache_set($relation['object']->getObjectId(), $relation['object']);
      }
    }

  }

  return $reply;
}

/**
 * Get item from static cache.
 */
function ting_cache_get($id, $collection = FALSE) {
  $cid = 'ting-' . ($collection ? 'collection' : 'object') . ':' . $id;
  $ttl = variable_get('ting_cache_lifetime', TING_DEFAULT_CACHE_LIFETIME);

  if ($ttl) {
    $cache = cache_get($cid);
    if ($cache && ($cache->expire > REQUEST_TIME)) {
      return $cache->data;
    }
    return NULL;
  }
  else {
    // Without proper caching, use a request cache.
    $cache = &drupal_static('ting_cache_set');
    if (!isset($cache)) {
      $cache = array();
    }
    // Using array_key_exists, as we might contain NULL values (which is !isset()).
    if (array_key_exists($cid, $cache)) {
      return $cache[$cid];
    }
    return NULL;
  }
}

/**
 * Put item in the static cache.
 */
function ting_cache_set($id, $value) {
  $cid = 'ting-object:' . $id;
  $ttl = variable_get('ting_cache_lifetime', TING_DEFAULT_CACHE_LIFETIME);

  if ($ttl) {
    $cache = cache_set($cid, $value, 'cache', REQUEST_TIME + $ttl);
  }
  else {
    // Without proper caching, use a request cache.
    $cache = &drupal_static(__FUNCTION__);
    if (!isset($cache)) {
      $cache = array();
    }
    $cache[$cid] = $value;
  }
}

/**
 * Get recommendations for a given ISBN.
 *
 * @param string $isbn
 *  ISBN number to get recommendations from.
 * @param $numResults
 *  The number of results to return.
 * @return array
 *  An array of TingClientObjectRecommendation objects.
 */
function ting_get_object_recommendations($isbn, $num_results = 10) {
  $recommendation_url = ting_get_request_factory()->getRequestUrl('recommendation');

  if (empty($recommendation_url)) {
    return FALSE;
  }

  $request = ting_get_request_factory()->getObjectRecommendationRequest();
  $request->setIsbn($isbn);
  $request->setNumResults($num_results);
  return ting_execute($request);
}

/**
 * Retrieves an initialized Ting client request factory.
 *
 * @return TingRequestInterface
 */
function ting_get_request_factory($type) {
  // Type => URL setting.
  $url_variables = &drupal_static(__FUNCTION__);

  if (empty($url_variables)) {
    $url_variables = array(
      'search' => 'ting_search_url',
      'object' => 'ting_search_url',
      'collection' => 'ting_search_url',
      'recommendation' => 'ting_recommendation_url',
      'relatedObjects' => 'ting_search_url',
    );

    // Add more urls from other modules.
    $additional_urls = module_invoke_all('ting_request_factory');
    if (!empty($additional_urls)) {
      $url_variables = array_merge($url_variables, $additional_urls);
    }
  }

  // Classname for request object.
  // ex: TingSearchRequest, TingObjectRequest, TingScanRequest, etc.
  $class_name = 'Ting' . ucfirst($type) . 'Request';

  if (class_exists($class_name)) {
    // Get settings from drupal.
    $url = variable_get($url_variables[$type], FALSE);
    $agency = variable_get('ting_agency', FALSE);
    $profile = variable_get('ting_search_profile', '');

    $request = new $class_name($url, $agency, $profile);
    return $request;
  }
  // TODO: Throw an error if class does not exist.
  return NULL;
}

function ting_add_relations($request, $type = 'full') {
  $request->setAllRelations(TRUE);
  $request->setRelationData($type);
  return $request;
}

/**
 * Perform a request against Ting and perform error handling if necessary.
 *
 * @param object $request
 *   The request.
 *
 * @return mixed
 *   Result of the request or false if an error occurs.
 */
function ting_execute($request) {
  if (variable_get('ting_filter_by_local_holdings', 0) && $request->getResultClass() == 'TingSearchResult') {
    // Limit the request from the local library. From well 3.5 each
    // library is no longer isolated.
    $request->setQueryPrefix('holdingsitem.agencyid=' . $request->getAgency());
  }

  try {
    timer_start('ting');
    $response = ting_get_client()->execute($request);
    timer_stop('ting');

    return $response;
  }
  catch (TingClientException $e) {
    timer_stop('ting');
    watchdog('ting client', 'Error performing request: ' . $e->getMessage(), NULL, WATCHDOG_NOTICE, 'http://' . $_SERVER["HTTP_HOST"] . request_uri());
    return FALSE;
  }
}

/**
 * Retrieves an initialized Ting client with appropriate request adapter and logger
 *
 * @return TingClient
 */
function ting_get_client() {
  static $client;

  if (!isset($client)) {
    $logger = new TingClientVoidLogger();
    if (variable_get('ting_enable_logging', FALSE)) {
      $logger = new TingClientDrupalWatchDogLogger();
    }
    $client = new TingClient($logger);
  }

  return $client;
}

/**
 * Use OpenScan to search for keyword, check
 * http://oss.dbc.dk/twiki/bin/view/Databroend/OpenSearchDocIndexes
 * for which phrase index to search, default is 'anyIndexes'
 *
 * @param string $query The prefix to scan for
 * @param string $phrase Which phrase index to search
 * @param int $num_results The numver of results to return
 * @return TingClientScanResult
 */
function ting_do_scan($query, $phrase = 'anyIndexes', $num_results = 10) {
  $request = ting_get_request_factory('scan');
  if (!$request) {
    return NULL;
  }

  $request->setField('phrase.' . $phrase);
  $request->setLower($query);
  $request->setLimit($num_results);

  return ting_execute($request);
}

/**
 * @param object $request - The TingClient object
 * @return TingClientScanRequest
 */
function ting_add_agency(TingClientScanRequest $request) {
  $agency = variable_get('ting_agency', FALSE);
  if ($agency) {
    $request->setAgency($agency);
  }
  return $request;
}

/**
 * @param string $word The word to get spell suggestions for
 * @param $num_results The number of results to return
 * @return array An array of TingClientSpellSuggestion objects
 */
function ting_get_spell_suggestions($word, $num_results = 10) {
  $request = ting_get_request_factory('spell');

  if (empty($request)) {
    return FALSE;
  }

  $request->setWord($word);
  $request->setLimit($num_results);
  return ting_execute($request);
}

